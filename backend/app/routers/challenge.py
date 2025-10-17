from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from datetime import date, datetime
from ..db import SessionLocal
from .. import models
from ..schemas import ChallengeResp

router = APIRouter(prefix="/challenge", tags=["challenge"])

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@router.get("/today", response_model=ChallengeResp)
def challenge_today(db: Session = Depends(get_db)):
    today = date.today()
    ch = db.get(models.Challenge, today)
    if not ch:
        ch = models.Challenge(day=today, seed="ABC123")  # seed any way you like
        db.add(ch)
        db.commit()
        db.refresh(ch)
    expires = datetime.combine(today, datetime.max.time()).replace(microsecond=0).isoformat() + "Z"
    return {"day": ch.day, "seed": ch.seed, "expiresAt": expires}