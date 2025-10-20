from fastapi.testclient import TestClient

def test_me_with_bearer_token(client: TestClient, unique_device_id, display_name):
    # register/login to get a jwt
    r = client.post(
        "/auth/register-or-login",
        json={"device_id": unique_device_id, "display_name": display_name},
    )
    assert r.status_code in (200, 201)
    payload = r.json()
    pid, jwt = payload["playerId"], payload["jwt"]

    # call /me with Authorization header
    r2 = client.get("/me", headers={"Authorization": f"Bearer {jwt}"})
    assert r2.status_code == 200
    assert r2.json()["playerId"] == pid
