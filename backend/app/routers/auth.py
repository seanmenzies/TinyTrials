from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from uuid import uuid4
from ..db import SessionLocal
from .. import models
from ..schemas import RegisterReq, AuthResp
from ..security import issue_jwt

router = APIRouter(prefix="/auth", tags=["auth"])

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@router.post("/register", response_model=AuthResp)
def register(req: RegisterReq, db: Session = Depends(get_db)):
    # naive: create a player every time for demo; in practice upsert by device_id
    player = models.Player(display_name=req.display_name or f"Player-{str(uuid4())[:8]}")
    db.add(player)
    db.commit()
    db.refresh(player)
    token = issue_jwt(str(player.id))
    return {"playerId": str(player.id), "jwt": token}