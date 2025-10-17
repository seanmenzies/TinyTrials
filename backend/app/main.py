from fastapi import FastAPI
from .db import init_db
from .routers import health, auth, challenge, scores

init_db()
app = FastAPI(title="TinyTrials API")

app.include_router(health.router)
app.include_router(auth.router)
app.include_router(challenge.router)
app.include_router(scores.router)