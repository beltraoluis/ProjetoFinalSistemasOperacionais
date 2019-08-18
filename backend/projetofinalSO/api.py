from flask import Flask, request, Response
from flask_cors import CORS

app = Flask(__name__)
cors = CORS(app, resources={r"/reset": {"origins": "*"}})
commands = []


@app.route("/")
def app_help():
    return "server ok", 200


@app.route("/commands", methods=["GET"])
def get_commands():
    if request.method == "GET":
        return str(commands)+";", 200
    else:
        return "invalid method " + request.method, 405


@app.route("/set/<value>", methods=["POST"])
def set_value(value):
    try:
        value = int(value)
    except():
        return "Value is not a int!", 400
    if request.method == "POST":
        commands.append(value)
        return "write " + str(value), 202
    else:
        return "invalid method " + request.method, 405


@app.route("/reset", methods=["DELETE"])
def reset():
    if request.method == "DELETE":
        commands.clear()
        return "cleared", 202
    else:
        return "invalid method " + request.method, 405

