from __future__ import annotations

import uuid
from datetime import date, datetime

from sqlalchemy import (
    Boolean,
    Date,
    DateTime,
    ForeignKey,
    Integer,
    String,
    func,
    Index,
)
from sqlalchemy.dialects.postgresql import UUID as PG_UUID
from sqlalchemy.orm import Mapped, mapped_column, relationship

from .db import Base


class Player(Base):
    __tablename__ = "players"

    id: Mapped[uuid.UUID] = mapped_column(PG_UUID(as_uuid=True), primary_key=True, default=uuid.uuid4)
    device_id: Mapped[str] = mapped_column(String, unique=True, index=True)
    display_name: Mapped[str] = mapped_column(String(20), nullable=False)
    created_at: Mapped[datetime] = mapped_column(DateTime(timezone=True), server_default=func.now())

    # Association objects (with extra fields like assigned_at/completed)
    player_challenges: Mapped[list["PlayerChallenge"]] = relationship(
        back_populates="player", cascade="all, delete-orphan"
    )
    # secondary tells SQL which table joins challenge & player
    challenges: Mapped[list["Challenge"]] = relationship(
        secondary="player_challenges", viewonly=True
    )

    scores: Mapped[list["Score"]] = relationship(
        back_populates="player", cascade="all, delete-orphan"
    )


class Challenge(Base):
    """
    One daily challenge per calendar day.
    Using 'day' (DATE) as the PK keeps the schema simple for daily leaderboards.
    """
    __tablename__ = "challenges"

    day: Mapped[date] = mapped_column(Date, primary_key=True)
    seed: Mapped[str] = mapped_column(String, nullable=False)
    # back_populate denotes which attribute in the related table represents the inverse side; should be mirrored by the other class
    player_challenges: Mapped[list["PlayerChallenge"]] = relationship(
        back_populates="challenge", cascade="all, delete-orphan"
    )
    # secondary tells SQL which table joins challenge & player
    players: Mapped[list["Player"]] = relationship(
        secondary="player_challenges", viewonly=True
    )

    scores: Mapped[list["Score"]] = relationship(
        back_populates="challenge", cascade="all, delete-orphan"
    )


class PlayerChallenge(Base):
    """
    Association-object table between Player and Challenge (day),
    carrying extra state like 'assigned_at' and 'completed'.
    """
    __tablename__ = "player_challenges"

    player_id: Mapped[uuid.UUID] = mapped_column(
        PG_UUID(as_uuid=True),
        ForeignKey("players.id", ondelete="CASCADE"),
        primary_key=True,
    )
    challenge_day: Mapped[date] = mapped_column(
        Date,
        ForeignKey("challenges.day", ondelete="CASCADE"),
        primary_key=True,
    )
    assigned_at: Mapped[datetime] = mapped_column(
        DateTime(timezone=True), server_default=func.now()
    )
    completed: Mapped[bool] = mapped_column(Boolean, nullable=False, default=False)

    player: Mapped["Player"] = relationship(back_populates="player_challenges")
    challenge: Mapped["Challenge"] = relationship(back_populates="player_challenges")


class Score(Base):
    __tablename__ = "scores"
    __table_args__ = (
        Index("ix_scores_day_score", "challenge_day", "score"),
    )

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    player_id: Mapped[uuid.UUID] = mapped_column(
        PG_UUID(as_uuid=True),
        ForeignKey("players.id", ondelete="CASCADE"),
        nullable=False,
    )
    challenge_day: Mapped[date] = mapped_column(
        Date,
        ForeignKey("challenges.day", ondelete="CASCADE"),
        nullable=False,
    )
    score: Mapped[int] = mapped_column(Integer, nullable=False)
    checksum: Mapped[str] = mapped_column(String, nullable=False)
    created_at: Mapped[datetime] = mapped_column(DateTime(timezone=True), server_default=func.now())

    player: Mapped["Player"] = relationship(back_populates="scores")
    challenge: Mapped["Challenge"] = relationship(back_populates="scores")
