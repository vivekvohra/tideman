# Use a lightweight Python image
FROM python:3.12-slim

# Ensure Python behaves well in containers
ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

# Install system deps (for building wheels)
RUN apt-get update -y && apt-get install -y --no-install-recommends \
    build-essential \
 && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY . .

# Expose Flask's port
EXPOSE 5000

# Run Gunicorn serving Flask's app object from app.py
CMD ["gunicorn", "-b", "0.0.0.0:5000", "app:app"]
