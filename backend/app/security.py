from datetime import datetime, timedelta, timezone
from jose import jwt
from .config import settings

def issue_jwt(player_id: str) -> str:
    exp = datetime.now(timezone.utc) + timedelta(minutes=settings.jwt_exp_minutes)
    payload = {"sub": player_id, "exp": exp}
    return jwt.encode(payload, settings.jwt_secret, algorithm=settings.jwt_alg)