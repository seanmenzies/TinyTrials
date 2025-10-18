@echo off
setlocal ENABLEDELAYEDEXPANSION

REM === CONFIG ===
set API_HOST=127.0.0.1      REM Use 0.0.0.0 if others on your LAN will connect
set API_PORT=8000
set PYTHON=python           REM Or full path: C:\Python312\python.exe
set ENTRY=run_server.py     REM Your entrypoint script
set LOG=%TEMP%\tinytrials_api.log

REM Optional: activate virtualenv if you have one
if exist ".venv\Scripts\activate.bat" (
  call ".venv\Scripts\activate.bat"
)

REM Clear old log
if exist "%LOG%" del "%LOG%" >nul 2>&1

echo.
echo === TinyTrials API ===
echo Host: %API_HOST%  Port: %API_PORT%
echo Log:  %LOG%
echo.

REM --- START server in background and capture PID via PowerShell ---
for /f %%p in ('
  powershell -NoProfile -Command ^
    "$p = Start-Process -FilePath '%PYTHON%' -ArgumentList '%ENTRY%' -PassThru -WindowStyle Hidden -RedirectStandardOutput '%LOG%' -RedirectStandardError '%LOG%'; ^
     $p.Id"
') do set PID=%%p

if "%PID%"=="" (
  echo [ERROR] Failed to start Python process.
  echo See log: %LOG%
  goto :ENDFAIL
)

echo [INFO] Server starting... (PID %PID%)
REM Give it a moment to boot
timeout /t 2 >nul

REM --- HEALTH CHECK ---
powershell -NoProfile -Command ^
  "try { iwr http://%API_HOST%:%API_PORT%/docs -UseBasicParsing -TimeoutSec 3 ^| out-null; exit 0 } catch { exit 1 }"
if errorlevel 1 (
  echo [FAILED] Server did not respond on http://%API_HOST%:%API_PORT% .
  echo          Check the log: %LOG%
  goto :WAIT_AND_STOP
) else (
  echo [OK] Server is running at: http://%API_HOST%:%API_PORT%/
)

echo.
echo Press any key to stop the server...
pause >nul

:WAIT_AND_STOP
REM If still running, stop it; if not, we’ll just report.
powershell -NoProfile -Command ^
  "try { $p = Get-Process -Id %PID% -ErrorAction Stop; Stop-Process -Id %PID% -Force; 'stopped' } catch { 'notrunning' }" > "%TEMP%\stop_state.txt"
set /p STOPSTATE=<"%TEMP%\stop_state.txt"
del "%TEMP%\stop_state.txt" >nul 2>&1

if /I "%STOPSTATE%"=="stopped" (
  echo [OK] Server stopped.
) else (
  echo [INFO] Server was not running (it may have crashed or exited).
)

echo.
echo Tail of log (last 20 lines):
powershell -NoProfile -Command "(Get-Content -Path '%LOG%' -ErrorAction SilentlyContinue | Select-Object -Last 20) -join \"`n\""
goto :ENDOK

:ENDFAIL
echo.
echo Press any key to close...
pause >nul
exit /b 1

:ENDOK
echo.
echo Press any key to close this window...
pause >nul
exit /b 0
