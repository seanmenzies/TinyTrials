import os
import uuid
import pytest
from fastapi.testclient import TestClient
from app.main import app

@pytest.fixture(scope="session")
def client():
    # Uses the same app instance your service runs with
    return TestClient(app)

@pytest.fixture
def unique_device_id():
    # Ensure idempotency test doesn’t collide across runs
    return f"test-device-{uuid.uuid4().hex[:12]}"

@pytest.fixture
def display_name():
    return "PyTestUser"
