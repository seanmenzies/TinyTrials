# tests/conftest.py
import os
import time
import uuid
import pytest
from sqlalchemy import create_engine, text, inspect
from sqlalchemy.orm import sessionmaker

# --- ensure env (inside container, 'db' is resolvable) ---
os.environ.setdefault("JWT_SECRET", "test_only_secret")
os.environ.setdefault("JWT_ALG", "HS256")
os.environ.setdefault("JWT_EXP_MINUTES", "60")

# If your api service gets DATABASE_URL via compose, you can omit the next line.
# Otherwise, uncomment to be explicit for tests:
# os.environ.setdefault("DATABASE_URL", "postgresql+psycopg://postgres:postgres@db:5432/tinytrials")

# --- import DB base/engine & models BEFORE importing app ---
import app.models  # registers Player, etc. on Base.metadata
from app.db import Base, engine  # same engine your app uses

# Wait for DB to be ready (quick retry loop)
for _ in range(30):
    try:
        with engine.connect() as conn:
            conn.execute(text("SELECT 1;"))
        break
    except Exception:
        time.sleep(0.5)
else:
    raise RuntimeError("DB never became ready")

# Create schema if missing
Base.metadata.create_all(bind=engine)

# Optional: assert tables exist (helps catch multiple-Base mistakes)
insp = inspect(engine)
assert "players" in insp.get_table_names(), f"players table missing: {insp.get_table_names()}"

# Now it's safe to import the app and create a client
from fastapi.testclient import TestClient
from app.main import app

@pytest.fixture(scope="session")
def client():
    return TestClient(app)

@pytest.fixture
def unique_device_id():
    return f"test-device-{uuid.uuid4().hex[:12]}"

@pytest.fixture
def display_name():
    return "PyTestUser"
