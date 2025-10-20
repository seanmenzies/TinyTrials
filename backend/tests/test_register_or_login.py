from fastapi.testclient import TestClient

def test_register_then_idempotent_login(client: TestClient, unique_device_id, display_name):
    # first call: create or login
    r1 = client.post(
        "/auth/register-or-login",
        json={"device_id": unique_device_id, "display_name": display_name},
    )
    assert r1.status_code in (200, 201)
    data1 = r1.json()
    assert "playerId" in data1 and "jwt" in data1
    player_id_1 = data1["playerId"]

    # second call with same device_id should return SAME playerId
    r2 = client.post(
        "/auth/register-or-login",
        json={"device_id": unique_device_id, "display_name": display_name + "_changed"},
    )
    assert r2.status_code in (200, 201)
    data2 = r2.json()
    assert data2["playerId"] == player_id_1
    assert "jwt" in data2
