# ---- build C++ binary ----
FROM gcc:13 AS build
WORKDIR /app
COPY tideman*.cpp ./tideman.cpp
RUN g++ -O2 -std=c++17 tideman.cpp -o tideman

# ---- runtime: Flask API + Gunicorn ----
FROM python:3.11-slim
WORKDIR /app

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1 \
    TIDEMAN_BIN=/app/tideman

# (optional) slim OS utilities
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# bring in compiled binary + app sources
COPY --from=build /app/tideman /app/tideman
COPY app.py index.html /app/

EXPOSE 5000
CMD ["gunicorn","-w","2","-b","0.0.0.0:5000","app:app"]
