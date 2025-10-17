from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from datetime import date
from jose import jwt, JWTError
from ..db import SessionLocal
from .. import models
from ..schemas import ScoreSubmit, ScoreItem
from ..config import settings

router = APIRouter(prefix="/scores", tags=["scores"])

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

def require_player_id(authorization: str | None) -> str:
    if not authorization or not authorization.lower().startswith("bearer "):
        raise HTTPException(401, "Missing bearer token")
    token = authorization.split(" ", 1)[1]
    try:
        payload = jwt.decode(token, settings.jwt_secret, algorithms=[settings.jwt_alg])
        return payload["sub"]
    except JWTError:
        raise HTTPException(401, "Invalid token")

@router.post("", status_code=201)
def submit_score(body: ScoreSubmit, db: Session = Depends(get_db), authorization: str | None = None):
    player_id = require_player_id(authorization)
    # TODO: verify checksum server-side based on shared secret and inputs
    score = models.Score(player_id=player_id, day=body.day, score=body.score, checksum=body.checksum)
    db.add(score)
    db.commit()
    return {"ok": True}

@router.get("/global", response_model=list[ScoreItem])
def global_scores(day: date, limit: int = 50, db: Session = Depends(get_db)):
    q = (db.query(models.Score, models.Player.display_name)
           .join(models.Player, models.Player.id == models.Score.player_id)
           .filter(models.Score.day == day)
           .order_by(models.Score.score.desc(), models.Score.id.desc())
           .limit(min(100, max(1, limit))))
    items = []
    for s, name in q.all():
        items.append(ScoreItem(playerName=name, score=s.score, createdAt=s.created_at.isoformat()))
    return items