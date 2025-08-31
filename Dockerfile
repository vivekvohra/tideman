FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

# build tools for python wheels + g++ for C++
RUN apt-get update -y && apt-get install -y --no-install-recommends \
    build-essential g++ \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Python deps
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# App source
COPY . .

# --- C++ build step ---
# If there are multiple .cpp files, this will compile them all into one binary
# named 'tideman'. If none exist, it will just skip.
RUN if ls *.cpp >/dev/null 2>&1; then \
      echo "Building C++: *.cpp -> /app/tideman" && \
      g++ -O2 -std=c++17 -o /app/tideman *.cpp; \
    else \
      echo "No .cpp files found, skipping C++ build"; \
    fi

# Expose internal Flask/Gunicorn port
EXPOSE 5000

# Run Flask via Gunicorn
CMD ["gunicorn", "-b", "0.0.0.0:5000", "app:app"]
