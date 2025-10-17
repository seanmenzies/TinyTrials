from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, DeclarativeBase
from .config import settings

class Base(DeclarativeBase):
    pass

engine = create_engine(settings.database_url, pool_pre_ping=True)
SessionLocal = sessionmaker(bind=engine, autoflush=False, autocommit=False)

# Dev-time “migrations”: create tables on container start
def init_db():
    from . import models  # ensure models imported
    Base.metadata.create_all(bind=engine)

if __name__ == "__main__":
    init_db()