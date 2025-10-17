from pydantic import BaseModel, Field
from datetime import date

class RegisterReq(BaseModel):
    device_id: str
    display_name: str

class AuthResp(BaseModel):
    playerId: str
    jwt: str

class ChallengeResp(BaseModel):
    day: date
    seed: str
    expiresAt: str

class ScoreSubmit(BaseModel):
    day: date
    score: int = Field(ge=0)
    checksum: str

class ScoreItem(BaseModel):
    playerName: str
    score: int
    createdAt: str