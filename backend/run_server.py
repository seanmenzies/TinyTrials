import os, uvicorn
os.environ.setdefault("API_HOST", "127.0.0.1")
os.environ.setdefault("API_PORT", "8000")
uvicorn.run("app.main:app", host=os.environ["API_HOST"], port=int(os.environ["API_PORT"]))