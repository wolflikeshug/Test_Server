from flask import Flask, render_template, request, redirect, url_for, session, make_response
from flask_session import Session
import json
import configparser
import socket
import binascii
import time

app = Flask(__name__)
app.config['SECRET_KEY'] = 'mysecretkey'
app.config['SESSION_TYPE'] = 'filesystem'
Session(app)


def user_login(username, password):
    with open('users.json', 'r') as f:
        users = json.load(f)
    if username in users and users[username] == password:
        return True
    else:
        return False

def get_config():
    # Create a ConfigParser object
    config = configparser.ConfigParser()
    
    # Read the configuration file
    config.read('config.ini')

    # Create an empty dictionary to store the configuration values
    config_dict = {}

    # Get the configuration values and add them to the dictionary
    config_dict['QB_server_c'] = config.get('TM_SERVER', 'QB_server_c')
    config_dict['QB_port_c'] = config.getint('TM_SERVER', 'QB_port_c')
    config_dict['QB_server_py'] = config.get('TM_SERVER', 'QB_server_py')
    config_dict['QB_port_py'] = config.getint('TM_SERVER', 'QB_port_py')
    config_dict['server_port'] = config.getint('TM_SERVER', 'server_port')

    # Return the configuration dictionary
    return config_dict

cfg = get_config()

def request_question(server_address, server_port, questions):
    # Create a socket object
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect to the server
    sock.bind((server_address, server_port))

    # Start listening for connections
    sock.listen(1)
    print("Waiting for connection...")

    # Wait for a connection
    conn, addr = sock.accept()
    print("Connected by", addr)

    # Receive data and print
    while True:
        data = conn.recv(1024)
        if not data:
            conn.send('1'.encode('ascii'))
            break
        conn.send(('0 \n').encode('ascii'))
        time.sleep(0.5)
        message = data.decode('ascii')
        print("Received message:", message)

    message = questions
    # Send the question request to the server
    sock.send(calculate_crc32(questions) + questions.encode('ascii'))

    # Receive the question from the server
    question = sock.recv(1024).decode('ascii')

    # Close the connection
    sock.close()

    # Return the question
    return question

@app.route('/')
def login():
    return render_template('login.html')

@app.route('/quiz', methods=['POST'])
def quiz():
    username = request.form['username']
    password = request.form['password']
    if (user_login(username, password)):
        session['username'] = username
        response = make_response(render_template('quiz.html', username=username
                                                 ))
        response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0'
        return response
    else:
        return redirect(url_for('login'))

@app.route('/result', methods=['POST'])
def result():
    score = 0
    choice_question_c = request.form['single_choice_c']
    choice_question_py = request.form['single_choice_py']
    multiple_choice_c = request.form.getlist('multiple_choice_c')
    multiple_choice_py = request.form.getlist('multiple_choice_py')
    coding_question_c = request.form['coding_question_c']
    coding_question_py = request.form['coding_question_py']

    if choice_question_c == 'a':
        score += 1
    if set(multiple_choice_c) == set(['a', 'b', 'c', 'd']):
        score += 1
    if coding_question_c.strip() == '12345':
        score += 1

    username = session['username']
    response = make_response(render_template('result.html', score=score, username=username))
    response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0'
    return response
if __name__ == '__main__':
    app.run(debug=True)
