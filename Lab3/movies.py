from flask import Flask, Response, request, jsonify, json
app = Flask(__name__)

movies = []
movie_id = 0

# POST
@app.route("/movies", methods=["POST"])
def post_movie():
    global movies
    global movie_id
    params = request.get_json(silent=True)
    if not params or params.get('nume') is None:
        return Response(status=400)

    new_movie = {"id": movie_id,
        "nume": params['nume']}

    movie_id += 1

    movies.append(new_movie)

    return Response(status=201)

# GET
@app.route("/movies", methods=["GET"])
def get_movies():
    global movies
    return jsonify(movies), 200

@app.route("/movie/<int:ID>", methods=["GET"])
def get_movie_by_id(ID):
    global movies
    for movie in movies:
        if movie['id'] == ID:
            return jsonify(movie), 200

    return Response(status='404')

# PUT
@app.route("/movie/<int:ID>", methods=["PUT"])
def update_movie(ID):
    global movies
    json_data = request.json
    if json_data and "nume" in json_data:
        for movie in movies:
            if movie['id'] == ID:
                movie['nume'] = json_data["nume"]
                return Response(status='200')
        return Response(status='404')
    else:
        return Response(status='400')

# DELETE
@app.route("/reset", methods=["DELETE"])
def delete_movies():
    global movies
    global movie_id
    movies.clear()
    movie_id = 0
    return Response(status='200')

@app.route("/movie/<int:ID>", methods=["DELETE"])
def delete_movie(ID):
    global movies
    for movie in movies:
        if movie['id'] == ID:
            del movies[ID]
            return Response(status='200')
        if ID > len(movies):
            return Response(status='404')


if __name__ == '__main__':
    app.run('0.0.0.0', debug=True)