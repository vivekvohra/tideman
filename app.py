import os, re, tempfile, subprocess
from pathlib import Path
from flask import Flask, request, jsonify

app = Flask(__name__)

# Resolve the EXE relative to this file (works even if you launch python from elsewhere)
APP_DIR = Path(__file__).resolve().parent

# 1) Default to tideman.exe on Windows, ./tideman on Unix
_default_bin = "tideman.exe" if os.name == "nt" else "./tideman"

# 2) Allow override via env, else use file-relative default
CPP_BIN = os.environ.get("TIDEMAN_BIN", str((APP_DIR / _default_bin)))

print(f"[Flask] CWD          = {Path.cwd()}")
print(f"[Flask] APP_DIR      = {APP_DIR}")
print(f"[Flask] Using CPP_BIN= {CPP_BIN}  (exists={Path(CPP_BIN).is_file()})")

WINNER_RE = re.compile(r"Winner:\s*(.+)")


@app.post("/api/tideman")
def run_tideman():
    """
    Send CSV either as:
      - multipart/form-data with 'file'
      - JSON: { "csv": "<CSV text>" }
    Returns: { winner, stdout, stderr }
    """
    csv_text = ""
    if "file" in request.files:
        csv_text = request.files["file"].read().decode("utf-8", "replace")
    else:
        data = request.get_json(silent=True) or {}
        csv_text = (data.get("csv") or "").strip()

    if not csv_text:
        return jsonify({"error": "CSV payload is empty"}), 400

    # write temp CSV
    with tempfile.NamedTemporaryFile(delete=False, suffix=".csv", mode="w") as tf:
        tf.write(csv_text)
        tf.flush()
        path = tf.name

    try:
        proc = subprocess.run(
            [CPP_BIN, f"--file={path}"],
            capture_output=True, text=True, timeout=10
        )
        out, err, rc = proc.stdout.strip(), proc.stderr.strip(), proc.returncode
        m = WINNER_RE.search(out)
        winner = m.group(1).strip() if m else None
        if rc != 0 or not winner:
            return jsonify({"error": "tideman failed", "stdout": out, "stderr": err, "rc": rc}), 500
        return jsonify({"winner": winner, "stdout": out, "stderr": err})
    finally:
        try: os.remove(path)
        except OSError: pass

@app.get("/healthz")
def health():
    return jsonify({"ok": True})

from flask import send_from_directory


@app.route("/")
def home():
    return send_from_directory(".", "index.html")



if __name__ == "__main__":
    # Force localhost:5000 so your tests/HTML match
    app.run(host="0.0.0.0", port=5000, debug=True)
