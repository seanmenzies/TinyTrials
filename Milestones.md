# 🧩 Tiny Trials – Project Milestones

A lightweight Unreal + Web backend practice project focused on core client–server integration, leaderboards, and cloud saves.

---

## 🏁 Phase 1 — Core Setup
**Goal:** Get both environments talking locally.

### ✅ Tasks
- [X] Create GitHub repo `TinyTrials`
- [X] Initialize Unreal 5.4.4 project `TinyTrialsGame`
- [X] Create `backend/` folder with `FastAPI` template
- [X] Add `docker-compose.yml` (API + DB + optional Redis)
- [X] Define `.env` (DB URL, JWT secret, etc.)
- [ ] Set up DB migrations (Alembic / Prisma / Sequelize)
- [ ] Verify `GET /health` returns OK

### 📦 Deliverables
- Unreal project launches and compiles
- API runs locally via `docker-compose up`
- `/health` endpoint reachable from Unreal HTTP module

---

## 🧠 Phase 2 — Authentication & Player Identity
**Goal:** Minimal but secure player identity.

### ✅ Tasks
- [ ] Implement `POST /auth/register` (register device or email)
- [ ] Implement `POST /auth/refresh`
- [ ] Return `playerId` and `jwt` token
- [ ] In Unreal, store token in `SaveGame` slot
- [ ] Auto-register if token not found on startup

### 📦 Deliverables
- Players can “log in” automatically
- JWT added to Unreal request headers

---

## ⚙️ Phase 3 — Daily Challenge Logic
**Goal:** Client fetches daily seed for deterministic gameplay.

### ✅ Tasks
- [ ] Create `challenges` table `{ day, seed }`
- [ ] Implement `GET /challenge/today`
- [ ] Unreal fetches and caches the seed
- [ ] Use seed in procedural level generation (or gameplay modifier)

### 📦 Deliverables
- Unreal can pull a unique daily configuration
- Challenge seed consistent for all players that day

---

## 🏆 Phase 4 — Leaderboard System
**Goal:** Submit and view scores.

### ✅ Tasks
- [ ] Create `scores` table `{ player_id, day, score, checksum }`
- [ ] Implement:
  - `POST /scores` (submit)
  - `GET /scores/global?limit=50`
  - `GET /scores/friends?limit=50`
- [ ] Add checksum verification (client-side hash with shared secret)
- [ ] Basic leaderboard UI in Unreal (scrolling widget or text list)

### 📦 Deliverables
- Scores submit and persist correctly
- Global leaderboard shows latest scores

---

## 🔔 Phase 5 — WebSocket Push (Live Updates)
**Goal:** Stream new scores without polling.

### ✅ Tasks
- [ ] Add WebSocket endpoint `/ws/scores`
- [ ] Broadcast new score events to all connected clients
- [ ] Unreal connects via `FWebSocketsModule`
- [ ] Update leaderboard dynamically

### 📦 Deliverables
- Clients see new scores in real time
- Stable WebSocket reconnection logic

---

## ☁️ Phase 6 — Cloud Saves
**Goal:** Persist small JSON player data remotely.

### ✅ Tasks
- [ ] Add `saves` table `{ player_id, data, updated_at }`
- [ ] Implement:
  - `GET /save`
  - `PUT /save`
- [ ] Unreal integrates with `USaveGame` serialization
- [ ] Sync settings and cosmetics between devices

### 📦 Deliverables
- Player settings stored server-side
- Data restored on next session

---

## 🔍 Phase 7 — Telemetry & Observability
**Goal:** Capture gameplay metrics and monitor backend.

### ✅ Tasks
- [ ] Add `POST /events` for lightweight telemetry
- [ ] Batch writes to DB/Redis
- [ ] Add request logging and simple analytics (FastAPI Middleware / Morgan)
- [ ] Optional: Export daily challenge stats to CSV

### 📦 Deliverables
- Events logged in DB
- Basic admin or CLI report (top players, avg score, etc.)

---

## 🚀 Phase 8 — Polish & Deployment
**Goal:** Production-ready deployment and testing.

### ✅ Tasks
- [ ] Write `Dockerfile` for API
- [ ] Create `docker-compose.prod.yml`
- [ ] Add CI pipeline (GitHub Actions)
- [ ] Deploy to free-tier cloud (Render / Railway / Fly.io)
- [ ] UE build packaged and tested against live API
- [ ] Document all endpoints in `API_REFERENCE.md`

### 📦 Deliverables
- Deployed backend
- Working UE client demo
- Screenshots + short clip for portfolio/interview

---

## 🧩 Optional Stretch Milestones
- [ ] Add Friends system (`/friends/add`, `/friends/list`)
- [ ] Add rate-limiting / anti-abuse middleware
- [ ] Implement cursor-based pagination for leaderboards
- [ ] Admin panel web dashboard (React or Wt)
- [ ] Cloud analytics dashboard (Grafana, OpenTelemetry)

---

## 🗂 Directory Overview
