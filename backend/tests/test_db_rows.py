import pytest
from sqlalchemy.orm import Session
from app.db import SessionLocal
from app import models

@pytest.mark.db
def test_player_row_inserted(unique_device_id, display_name):
    # This assumes your /register-or-login handler upserts by device_id
    # We’ll call the handler function via HTTP at higher layers, but here we just
    # verify the DB row exists after the previous tests (pytest order is not guaranteed).
    # So, do a fresh call here as well to ensure presence.
    from fastapi.testclient import TestClient
    from app.main import app
    client = TestClient(app)
    r = client.post("/auth/register-or-login", json={"device_id": unique_device_id, "display_name": display_name})
    assert r.status_code in (200, 201)
    pid = r.json()["playerId"]

    # Now check DB directly
    with SessionLocal() as db:  # type: Session
        row = db.query(models.Player).filter(models.Player.id == pid).one_or_none()
        assert row is not None
        assert isinstance(row.display_name, str)
