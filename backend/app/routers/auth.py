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

@router.post("/register-or-login", response_model=AuthResp)
def register_or_login(req: RegisterReq, db: Session = Depends(get_db)):
    # sanitize, @TODO make universal, importable config rules across whole pipeline
    name = (req.display_name or "").strip()[:32] or f"Player-{str(uuid4())[:8]}"

    player = db.query(models.Player).filter(models.Player.device_id == req.device_id).one_or_none()
    if not player:
        player = models.Player(device_id=req.device_id, display_name=name)
        db.add(player)
        db.commit()
        db.refresh(player)
    else:
        # update display name if user changed it
        if name and name != player.display_name:
            player.display_name = name
            db.commit()

    token = issue_jwt(str(player.id))
    return {"playerId": str(player.id), "jwt": token}