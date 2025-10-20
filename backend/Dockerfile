FROM python:3.12-slim

WORKDIR /app
# OS deps (psycopg2 needs these)
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc libpq-dev curl && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Dev/test extras
COPY requirements-dev.txt .
RUN pip install --no-cache-dir -r requirements-dev.txt

COPY . .

# Default: run the API (used by the api service)
CMD ["sh", "-c", "python -m app.db && uvicorn app.main:app --host 0.0.0.0 --port 8000 --proxy-headers --forwarded-allow-ips='*'"]
