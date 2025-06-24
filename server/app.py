from flask import Flask, request, jsonify
from flask_cors import CORS  # ✅ Enables frontend-backend communication
import subprocess
import os
import uuid

app = Flask(__name__)
CORS(app)  # ✅ Allows requests from localhost:3000

@app.route('/optimize', methods=['POST'])
def optimize():
    if 'file' not in request.files:
        return jsonify({"error": "No file uploaded"}), 400

    file = request.files['file']
    filename = f"input_{uuid.uuid4().hex[:8]}.txt"
    filepath = os.path.join(os.getcwd(), filename)
    file.save(filepath)

    try:
        # ✅ Safely read input before subprocess (avoids file access error)
        with open(filepath, 'r') as f:
            input_data = f.read()

        result = subprocess.run(
            ['./axis_optimizer'],
            input=input_data,
            text=True,
            capture_output=True,
            timeout=15
        )

        os.remove(filepath)

        if result.returncode != 0:
            return jsonify({"error": result.stderr or "C++ optimizer failed"}), 500

        lines = result.stdout.strip().split('\n')
        cost = float(lines[0])
        rectangle = []
        for line in lines[1:]:
            x1, y1, x2, y2 = map(float, line.strip().split())
            rectangle.append([[x1, y1], [x2, y2]])

        # ✅ Read building data again
        buildings = []
        for line in input_data.strip().split('\n')[1:]:  # skip the first line (n k)
            x, y, w = map(float, line.strip().split())
            buildings.append({'x': x, 'y': y, 'weight': w})
        raw_output = result.stdout.strip()

        return jsonify({
            "cost": cost,
            "rectangle": rectangle,
            "buildings": buildings,
            "raw_output": raw_output
        })


    except subprocess.TimeoutExpired:
        return jsonify({"error": "C++ code timed out"}), 500

if __name__ == '__main__':
    import os
port = int(os.environ.get("PORT", 5050))
app.run(debug=False, host="0.0.0.0", port=port)

