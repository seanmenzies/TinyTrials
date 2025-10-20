from fastapi import FastAPI
from .db import init_db
from .routers import health, auth, challenge, scores
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI(title="TinyTrials API")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # tighten later
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"])

app.include_router(health.router)
app.include_router(auth.router)
app.include_router(challenge.router)
app.include_router(scores.router)

@app.on_event("startup")
def _startup() -> None:
    init_db()