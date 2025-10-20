from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, DeclarativeBase, Session
from contextlib import contextmanager
from .config import settings

class Base(DeclarativeBase):
    pass

engine = create_engine(
    settings.database_url,
    pool_pre_ping=True,
    future=True,
)
SessionLocal = sessionmaker(bind=engine, autoflush=False, autocommit=False)

# Dev-time “migrations”: create tables on container start
def init_db():
    from . import models  # ensure models imported
    Base.metadata.create_all(bind=engine)

@contextmanager
def session_scope():
    db: Session = SessionLocal()
    try:
        yield db
        db.commit()
    except:
        db.rollback()
        raise
    finally:
        db.close()

# FastAPI dependency (preferred for routers)
def get_db():
    db: Session = SessionLocal()
    try:
        yield db
    finally:
        db.close()

if __name__ == "__main__":
    init_db()