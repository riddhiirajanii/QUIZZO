from flask import Flask, request, jsonify
from flask_cors import CORS
from sklearn.feature_extraction.text import TfidfVectorizer
import json

app = Flask(__name__)
CORS(app)

with open("C:/xampp/htdocs/quizzo/data/quiz_data.json") as f:
    quiz_data = json.load(f)

descriptions = [" ".join(quiz['tags']) for quiz in quiz_data]
vectorizer = TfidfVectorizer()
vectors = vectorizer.fit_transform(descriptions)

@app.route("/search")
def search():
    query = request.args.get("q", "").lower()
    results = []

    for quiz in quiz_data:
        title_match = query in quiz["title"].lower()
        tag_match = any(query in tag.lower() for tag in quiz.get("tags", []))
        if title_match or tag_match:
            results.append(quiz)

    return jsonify(results)

@app.route('/random')
def random_quiz():
    import random
    return jsonify(random.choice(quiz_data))

if __name__ == '__main__':
    app.run(debug=True, port=5000)
