from flask import Flask, render_template, request, redirect, url_for, session, make_response, flash
from flask_session import Session
import json
import configparser
import socket
import random
import socket
import time

app = Flask(__name__)
app.config['SECRET_KEY'] = 'tmserver'
app.config['SESSION_TYPE'] = 'filesystem'
Session(app)


def user_login(username, password):
    with open('users.json', 'r') as f:
        users = json.load(f)
    if username in users and users[username]['password'] == password:
        print('login success')
        return True
    else:
        print('login failed')
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
    config_dict['QB_port_c'] = int(config.getint('TM_SERVER', 'QB_port_c'))
    config_dict['QB_server_py'] = config.get('TM_SERVER', 'QB_server_py')
    config_dict['QB_port_py'] = int(config.getint('TM_SERVER', 'QB_port_py'))
    #config_dict['server_port'] = config.getint('TM_SERVER', 'server_port')

    # Return the configuration dictionary
    return config_dict

cfg = get_config()

def communicate_with_server(ip_address, port, message):
    # Set the server address and port number
    SERVER_ADDRESS = (ip_address, port)
    print(SERVER_ADDRESS)

    # Create a socket object
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind to the server address and port number
    sock.bind(SERVER_ADDRESS)

    # Start listening for connections
    sock.listen(1)
    print("Waiting for connection...")

    # Wait for a connection
    conn, addr = sock.accept()
    print("Connected by", addr)

    count = 0

    # Send data
    conn.send(message.encode('utf-8'))

    # Receive data
    while count < 3:
        data = conn.recv(262144)
        if data:
            response = data.decode('utf-8')
            conn.send("message received".encode('utf-8'))
            conn.close()
            sock.close()
            return response.rstrip('\n')
        else:
            count += 1
            time.sleep(2)
            try:
                conn.send(message.encode('utf-8'))
            except OSError as e:
                print (e)

    # No response received
    conn.close()
    sock.close()
    return ''

def request_question(username):
    with open('users.json', 'r') as f:
        users = json.load(f)
    if (users[username]['selected_questions'] == {}):
        cc1 = str(random.randint(1, 4))
        cc2 = str(random.randint(1, 4))
        while (cc2 == cc1):
            cc2 = str(random.randint(1, 4))
        cm1 = str(random.randint(1, 4))
        cm2 = str(random.randint(1, 4))
        while (cm2 == cm1):
            cm2 = str(random.randint(1, 4))
        pyc1 = str(random.randint(1, 4))
        pyc2 = str(random.randint(1, 4))
        while (pyc2 == pyc1):
            pyc2 = str(random.randint(1, 4))
        pym1 = str(random.randint(1, 4))
        pym2 = str(random.randint(1, 4))
        while (pym2 == pym1):
            pym2 = str(random.randint(1, 4))
        cco1 = str(random.randint(1, 3))
        pyco1 = str(random.randint(1, 3))

        message1 = cc1+" "+cc2+"|"+cm1+" "+cm2+"|"+cco1+'r' # r represents request questions
        message2 = pyc1+" "+pyc2+"|"+pym1+" "+pym2+"|"+pyco1+'r'

        responsec = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], message1)
        responsepy = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], message2)
        
        print(responsec)
        print(responsepy)

        users[username]['selected_questions']['c'] = json.loads(responsec)
        users[username]['selected_questions']['py'] = json.loads(responsepy)

        with open('users.json', 'w') as f:
            json.dump(users, f)

def request_answer(id, type, qtype):
    if (qtype == "choice"):
        if (type == "c"):
            response = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], str(id)+'x')
        elif (type == "py"):
            response = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], str(id)+'x')
    elif (qtype == "multi-choice"):
        if (type == "c"):
            response = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], str(id)+'y')
        elif (type == "py"):
            response = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], str(id)+'y')
    elif (qtype == "coding"):
        if (type == "c"):
            response = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], str(id)+'z')
        elif (type == "py"):
            response = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], str(id)+'z')

    return response

def submit_ans(ans, id, type, qtype, username, question_num):
    with open('users.json', 'r') as f:
        users = json.load(f)
    
    if (qtype == 'choice'):
        if (type == 'c'):
            response = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], ans+str(id)+'q')
        elif (type == 'py'):
            response = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], ans+str(id)+'q')
    elif (qtype == 'multi-choice'):
        if (type == 'c'):
            response = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], ans+str(id)+'m')
        elif (type == 'py'):
            response = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], ans+str(id)+'m')
    elif (qtype == 'coding'):
        if (type == 'c'):
            response = communicate_with_server(cfg['QB_server_c'], cfg['QB_port_c'], ans+str(id)+'c')
        elif (type == 'py'):
            response = communicate_with_server(cfg['QB_server_py'], cfg['QB_port_py'], ans+str(id)+'p')
    
    if (response == '0') and '0'not in users[username]['attempts'][type][qtype][question_num]['attempt_result']:
        users[username]['attempts'][type][qtype][question_num]['attempt_score'] =  3-(len(users[username]['attempts'][type][qtype][question_num]['attempt_answer']))
        users[username]['total_score'] += 3-(len(users[username]['attempts'][type][qtype][question_num]['attempt_answer']))

    users[username]['attempts'][type][qtype][question_num]['attempt_answer'].append(ans)
    users[username]['attempts'][type][qtype][question_num]['attempt_result'].append(response)


    
    with open('users.json', 'w') as f:
        json.dump(users, f)
    

@app.route('/')
def home():
    if 'username' in session:
        return redirect(url_for('quiz'))
    return redirect(url_for('login'))

@app.route('/login', methods=['GET', 'POST'])
def login():
    
    session.clear()
    return render_template('login.html')

@app.route('/manage', methods=['GET', 'POST'])
def manage():
    with open('users.json', 'r') as f:
        users = json.load(f)
    if users['alice']['selected_questions'] == {}:
        request_question('alice')
    if users['bob']['selected_questions'] == {}:
        request_question('bob')
    if users['chris']['selected_questions'] == {}:
        request_question('chris')
    return render_template('manage.html',\
                            total_score_alice = users['alice']['total_score'],\
                            total_score_bob = users['bob']['total_score'],\
                            total_score_chris = users['chris']['total_score'],\
                            question_1_alice = users['alice']['selected_questions']['c']['choice'][0]['question'],\
                            question_1_score_alice = users['alice']['attempts']['c']['choice'][0]['attempt_score'],\
                            question_1_attempt_alice = users['alice']['attempts']['c']['choice'][0]['attempt_answer'],\
                            question_2_alice = users['alice']['selected_questions']['c']['choice'][1]['question'],\
                            question_2_score_alice = users['alice']['attempts']['c']['choice'][1]['attempt_score'],\
                            question_2_attempt_alice = users['alice']['attempts']['c']['choice'][1]['attempt_answer'],\
                            question_3_alice = users['alice']['selected_questions']['py']['choice'][0]['question'],\
                            question_3_score_alice = users['alice']['attempts']['py']['choice'][0]['attempt_score'],\
                            question_3_attempt_alice = users['alice']['attempts']['py']['choice'][0]['attempt_answer'],\
                            question_4_alice = users['alice']['selected_questions']['py']['choice'][1]['question'],\
                            question_4_score_alice = users['alice']['attempts']['py']['choice'][1]['attempt_score'],\
                            question_4_attempt_alice = users['alice']['attempts']['py']['choice'][1]['attempt_answer'],\
                            question_5_alice = users['alice']['selected_questions']['c']['multi-choice'][0]['question'],\
                            question_5_score_alice = users['alice']['attempts']['c']['multi-choice'][0]['attempt_score'],\
                            question_5_attempt_alice = users['alice']['attempts']['c']['multi-choice'][0]['attempt_answer'],\
                            question_6_alice = users['alice']['selected_questions']['c']['multi-choice'][1]['question'],\
                            question_6_score_alice = users['alice']['attempts']['c']['multi-choice'][1]['attempt_score'],\
                            question_6_attempt_alice = users['alice']['attempts']['c']['multi-choice'][1]['attempt_answer'],\
                            question_7_alice = users['alice']['selected_questions']['py']['multi-choice'][0]['question'],\
                            question_7_score_alice = users['alice']['attempts']['py']['multi-choice'][0]['attempt_score'],\
                            question_7_attempt_alice = users['alice']['attempts']['py']['multi-choice'][0]['attempt_answer'],\
                            question_8_alice = users['alice']['selected_questions']['py']['multi-choice'][1]['question'],\
                            question_8_score_alice = users['alice']['attempts']['py']['multi-choice'][1]['attempt_score'],\
                            question_8_attempt_alice = users['alice']['attempts']['py']['multi-choice'][1]['attempt_answer'],\
                            question_9_alice = users['alice']['selected_questions']['c']['coding'][0]['question'],\
                            question_9_score_alice = users['alice']['attempts']['c']['coding'][0]['attempt_score'],\
                            question_9_attempt_alice = users['alice']['attempts']['c']['coding'][0]['attempt_answer'],\
                            question_10_alice = users['alice']['selected_questions']['py']['coding'][0]['question'],\
                            question_10_score_alice = users['alice']['attempts']['py']['coding'][0]['attempt_score'],\
                            question_10_attempt_alice = users['alice']['attempts']['py']['coding'][0]['attempt_answer'],\
                            question_1_bob = users['bob']['selected_questions']['c']['choice'][0]['question'],\
                            question_1_score_bob = users['bob']['attempts']['c']['choice'][0]['attempt_score'],\
                            question_1_attempt_bob = users['bob']['attempts']['c']['choice'][0]['attempt_answer'],\
                            question_2_bob = users['bob']['selected_questions']['c']['choice'][1]['question'],\
                            question_2_score_bob = users['bob']['attempts']['c']['choice'][1]['attempt_score'],\
                            question_2_attempt_bob = users['bob']['attempts']['c']['choice'][1]['attempt_answer'],\
                            question_3_bob = users['bob']['selected_questions']['py']['choice'][0]['question'],\
                            question_3_score_bob = users['bob']['attempts']['py']['choice'][0]['attempt_score'],\
                            question_3_attempt_bob = users['bob']['attempts']['py']['choice'][0]['attempt_answer'],\
                            question_4_bob = users['bob']['selected_questions']['py']['choice'][1]['question'],\
                            question_4_score_bob = users['bob']['attempts']['py']['choice'][1]['attempt_score'],\
                            question_4_attempt_bob = users['bob']['attempts']['py']['choice'][1]['attempt_answer'],\
                            question_5_bob = users['bob']['selected_questions']['c']['multi-choice'][0]['question'],\
                            question_5_score_bob = users['bob']['attempts']['c']['multi-choice'][0]['attempt_score'],\
                            question_5_attempt_bob = users['bob']['attempts']['c']['multi-choice'][0]['attempt_answer'],\
                            question_6_bob = users['bob']['selected_questions']['c']['multi-choice'][1]['question'],\
                            question_6_score_bob = users['bob']['attempts']['c']['multi-choice'][1]['attempt_score'],\
                            question_6_attempt_bob = users['bob']['attempts']['c']['multi-choice'][1]['attempt_answer'],\
                            question_7_bob = users['bob']['selected_questions']['py']['multi-choice'][0]['question'],\
                            question_7_score_bob = users['bob']['attempts']['py']['multi-choice'][0]['attempt_score'],\
                            question_7_attempt_bob = users['bob']['attempts']['py']['multi-choice'][0]['attempt_answer'],\
                            question_8_bob = users['bob']['selected_questions']['py']['multi-choice'][1]['question'],\
                            question_8_score_bob = users['bob']['attempts']['py']['multi-choice'][1]['attempt_score'],\
                            question_8_attempt_bob = users['bob']['attempts']['py']['multi-choice'][1]['attempt_answer'],\
                            question_9_bob = users['bob']['selected_questions']['c']['coding'][0]['question'],\
                            question_9_score_bob = users['bob']['attempts']['c']['coding'][0]['attempt_score'],\
                            question_9_attempt_bob = users['bob']['attempts']['c']['coding'][0]['attempt_answer'],\
                            question_10_bob = users['bob']['selected_questions']['py']['coding'][0]['question'],\
                            question_10_score_bob = users['bob']['attempts']['py']['coding'][0]['attempt_score'],\
                            question_10_attempt_bob = users['bob']['attempts']['py']['coding'][0]['attempt_answer'],\
                            question_1_chris = users['chris']['selected_questions']['c']['choice'][0]['question'],\
                            question_1_score_chris = users['chris']['attempts']['c']['choice'][0]['attempt_score'],\
                            question_1_attempt_chris = users['chris']['attempts']['c']['choice'][0]['attempt_answer'],\
                            question_2_chris = users['chris']['selected_questions']['c']['choice'][1]['question'],\
                            question_2_score_chris = users['chris']['attempts']['c']['choice'][1]['attempt_score'],\
                            question_2_attempt_chris = users['chris']['attempts']['c']['choice'][1]['attempt_answer'],\
                            question_3_chris = users['chris']['selected_questions']['py']['choice'][0]['question'],\
                            question_3_score_chris = users['chris']['attempts']['py']['choice'][0]['attempt_score'],\
                            question_3_attempt_chris = users['chris']['attempts']['py']['choice'][0]['attempt_answer'],\
                            question_4_chris = users['chris']['selected_questions']['py']['choice'][1]['question'],\
                            question_4_score_chris = users['chris']['attempts']['py']['choice'][1]['attempt_score'],\
                            question_4_attempt_chris = users['chris']['attempts']['py']['choice'][1]['attempt_answer'],\
                            question_5_chris = users['chris']['selected_questions']['c']['multi-choice'][0]['question'],\
                            question_5_score_chris = users['chris']['attempts']['c']['multi-choice'][0]['attempt_score'],\
                            question_5_attempt_chris = users['chris']['attempts']['c']['multi-choice'][0]['attempt_answer'],\
                            question_6_chris = users['chris']['selected_questions']['c']['multi-choice'][1]['question'],\
                            question_6_score_chris = users['chris']['attempts']['c']['multi-choice'][1]['attempt_score'],\
                            question_6_attempt_chris = users['chris']['attempts']['c']['multi-choice'][1]['attempt_answer'],\
                            question_7_chris = users['chris']['selected_questions']['py']['multi-choice'][0]['question'],\
                            question_7_score_chris = users['chris']['attempts']['py']['multi-choice'][0]['attempt_score'],\
                            question_7_attempt_chris = users['chris']['attempts']['py']['multi-choice'][0]['attempt_answer'],\
                            question_8_chris = users['chris']['selected_questions']['py']['multi-choice'][1]['question'],\
                            question_8_score_chris = users['chris']['attempts']['py']['multi-choice'][1]['attempt_score'],\
                            question_8_attempt_chris = users['chris']['attempts']['py']['multi-choice'][1]['attempt_answer'],\
                            question_9_chris = users['chris']['selected_questions']['c']['coding'][0]['question'],\
                            question_9_score_chris = users['chris']['attempts']['c']['coding'][0]['attempt_score'],\
                            question_9_attempt_chris = users['chris']['attempts']['c']['coding'][0]['attempt_answer'],\
                            question_10_chris = users['chris']['selected_questions']['py']['coding'][0]['question'],\
                            question_10_score_chris = users['chris']['attempts']['py']['coding'][0]['attempt_score'],\
                            question_10_attempt_chris = users['chris']['attempts']['py']['coding'][0]['attempt_answer'],\
                            )

@app.route('/quiz', methods=['GET', 'POST'])
def quiz():
    if 'username' not in session and 'username' not in request.form:
        return redirect(url_for('login'))
    
    if 'logout' in request.form:
        session.clear()
        flash('You have been logged out.', 'success')
        return redirect(url_for('login'))

    if 'username' not in session:
        username = request.form['username']
        password = request.form['password']
        if not user_login(username, password):
            return redirect(url_for('login'))
        session['username'] = username
    
    username = session['username']
    with open('users.json', 'r') as f:
        users = json.load(f)
        if (users[username]['selected_questions'] == {}):
            request_question(username)
    with open('users.json', 'r') as f:
        users = json.load(f)

    question_1_ans = ""
    question_2_ans = ""
    question_3_ans = ""
    question_4_ans = ""
    question_5_ans = ""
    question_6_ans = ""
    question_7_ans = ""
    question_8_ans = ""
    question_9_ans = ""
    question_10_ans = ""
    
    choice_1_attempt=len(users[username]['attempts']['c']['choice'][0]['attempt_answer'])
    if choice_1_attempt > 0:
        choice_1_ans=users[username]['attempts']['c']['choice'][0]['attempt_answer'][-1]
        choice_1_result=users[username]['attempts']['c']['choice'][0]['attempt_result'][-1]
        if choice_1_result == '0':
            choice_1_result = 'Correct'
        else:
            choice_1_result = 'Incorrect'
        choice_1_ans = choice_1_ans + "\tWhich is " + choice_1_result
    else:
        choice_1_ans='Haven\'t attempted yet'

    choice_2_attempt=len(users[username]['attempts']['c']['choice'][1]['attempt_answer'])
    if choice_2_attempt > 0:
        choice_2_ans=users[username]['attempts']['c']['choice'][1]['attempt_answer'][-1]
        choice_2_result=users[username]['attempts']['c']['choice'][1]['attempt_result'][-1]
        if choice_2_result == '0':
            choice_2_result = 'Correct'
        else:
            choice_2_result = 'Incorrect'
        choice_2_ans = choice_2_ans + "\tWhich is " + choice_2_result 
    else:
        choice_2_ans='Haven\'t attempted yet'

    choice_3_attempt=len(users[username]['attempts']['py']['choice'][0]['attempt_answer'])
    if choice_3_attempt > 0:
        choice_3_ans=users[username]['attempts']['py']['choice'][0]['attempt_answer'][-1]
        choice_3_result=users[username]['attempts']['py']['choice'][0]['attempt_result'][-1]
        if choice_3_result == '0':
            choice_3_result = 'Correct'
        else:
            choice_3_result = 'Incorrect'
        choice_3_ans = choice_3_ans + "\tWhich is " + choice_3_result
    else:
        choice_3_ans='Haven\'t attempted yet'

    choice_4_attempt=len(users[username]['attempts']['py']['choice'][1]['attempt_answer'])
    if choice_4_attempt > 0:
        choice_4_ans=users[username]['attempts']['py']['choice'][1]['attempt_answer'][-1]
        choice_4_result=users[username]['attempts']['py']['choice'][1]['attempt_result'][-1]
        if choice_4_result == '0':
            choice_4_result = 'Correct'
        else:
            choice_4_result = 'Incorrect'
        choice_4_ans = choice_4_ans + "\tWhich is " + choice_4_result
    else:
        choice_4_ans='Haven\'t attempted yet'

    multi_1_attempt=len(users[username]['attempts']['c']['multi-choice'][0]['attempt_answer'])
    if multi_1_attempt > 0:
        multi_1_ans=users[username]['attempts']['c']['multi-choice'][0]['attempt_answer'][-1]
        multi_1_result=users[username]['attempts']['c']['multi-choice'][0]['attempt_result'][-1]
        if multi_1_result == '0':
            multi_1_result = 'Correct'
        else:
            multi_1_result = 'Incorrect'
        multi_1_ans = multi_1_ans + "\tWhich is " + multi_1_result
    else:
        multi_1_ans='Haven\'t attempted yet'

    multi_2_attempt=len(users[username]['attempts']['c']['multi-choice'][1]['attempt_answer'])
    if multi_2_attempt > 0:
        multi_2_ans=users[username]['attempts']['c']['multi-choice'][1]['attempt_answer'][-1]
        multi_2_result=users[username]['attempts']['c']['multi-choice'][1]['attempt_result'][-1]
        if multi_2_result == '0':
            multi_2_result = 'Correct'
        else:
            multi_2_result = 'Incorrect'
        multi_2_ans = multi_2_ans + "\tWhich is " + multi_2_result
    else:
        multi_2_ans='Haven\'t attempted yet'

    multi_3_attempt=len(users[username]['attempts']['py']['multi-choice'][0]['attempt_answer'])
    if multi_3_attempt > 0:
        multi_3_ans=users[username]['attempts']['py']['multi-choice'][0]['attempt_answer'][-1]
        multi_3_result=users[username]['attempts']['py']['multi-choice'][0]['attempt_result'][-1]
        if multi_3_result == '0':
            multi_3_result = 'Correct'
        else:
            multi_3_result = 'Incorrect'
        multi_3_ans = multi_3_ans + "\tWhich is " + multi_3_result
    else:
        multi_3_ans='Haven\'t attempted yet'

    multi_4_attempt=len(users[username]['attempts']['py']['multi-choice'][1]['attempt_answer'])
    if multi_4_attempt > 0:
        multi_4_ans=users[username]['attempts']['py']['multi-choice'][1]['attempt_answer'][-1]
        multi_4_result=users[username]['attempts']['py']['multi-choice'][1]['attempt_result'][-1]
        if multi_4_result == '0':
            multi_4_result = 'Correct'
        else:
            multi_4_result = 'Incorrect'
        multi_4_ans = multi_4_ans + "\tWhich is " + multi_4_result
    else:
        multi_4_ans='Haven\'t attempted yet'

    coding_1_attempt=len(users[username]['attempts']['c']['coding'][0]['attempt_answer'])
    if coding_1_attempt > 0:
        coding_1_ans=users[username]['attempts']['c']['coding'][0]['attempt_answer'][-1]
        coding_1_result=users[username]['attempts']['c']['coding'][0]['attempt_result'][-1]
        if coding_1_result == '0':
            coding_1_result = 'Correct'
        else:
            coding_1_result = 'Incorrect with error: \n'+coding_1_result
        coding_1_ans = coding_1_ans + "\tWhich is " + coding_1_result
    else:
        coding_1_ans='Haven\'t attempted yet'

    coding_2_attempt=len(users[username]['attempts']['py']['coding'][0]['attempt_answer'])
    if coding_2_attempt > 0:
        coding_2_ans=users[username]['attempts']['py']['coding'][0]['attempt_answer'][-1]
        coding_2_result=users[username]['attempts']['py']['coding'][0]['attempt_result'][-1]
        if coding_2_result == '0':
            coding_2_result = 'Correct'
        else:
            coding_2_result = 'Incorrect with error: \n'+coding_2_result
        coding_2_ans = coding_2_ans + "\tWhich is " + coding_2_result
    else:
        coding_2_ans='Haven\'t attempted yet'

    if 'submit Q1' in request.form and choice_1_attempt < 3:
        submit_ans(request.form['single_choice_1'],users[username]['selected_questions']['c']['choice'][0]['id'],"c","choice",username,0)
        return redirect(url_for('quiz'))
    elif 'submit Q2' in request.form and choice_2_attempt < 3:
        submit_ans(request.form['single_choice_2'],users[username]['selected_questions']['c']['choice'][1]['id'],"c","choice",username,1)
        return redirect(url_for('quiz'))
    elif 'submit Q3' in request.form and choice_3_attempt < 3:
        submit_ans(request.form['single_choice_3'],users[username]['selected_questions']['py']['choice'][0]['id'],"py","choice",username,0)
        return redirect(url_for('quiz'))
    elif 'submit Q4' in request.form and choice_4_attempt < 3:
        submit_ans(request.form['single_choice_4'],users[username]['selected_questions']['py']['choice'][1]['id'],"py","choice",username,1)
        return redirect(url_for('quiz'))
    elif 'submit Q5' in request.form and multi_1_attempt < 3:
        submit_ans(''.join(request.form.getlist('multiple_choice_1')),users[username]['selected_questions']['c']['multi-choice'][0]['id'],"c","multi-choice",username,0)
        return redirect(url_for('quiz'))
    elif 'submit Q6' in request.form and multi_2_attempt < 3:
        submit_ans(''.join(request.form.getlist('multiple_choice_2')),users[username]['selected_questions']['c']['multi-choice'][1]['id'],"c","multi-choice",username,1)
        return redirect(url_for('quiz'))
    elif 'submit Q7' in request.form and multi_3_attempt < 3:
        submit_ans(''.join(request.form.getlist('multiple_choice_3')),users[username]['selected_questions']['py']['multi-choice'][0]['id'],"py","multi-choice",username,0)
        return redirect(url_for('quiz'))
    elif 'submit Q8' in request.form and multi_4_attempt < 3:
        submit_ans(''.join(request.form.getlist('multiple_choice_4')),users[username]['selected_questions']['py']['multi-choice'][1]['id'],"py","multi-choice",username,1)
        return redirect(url_for('quiz'))
    elif 'submit Q9' in request.form and coding_1_attempt < 3:
        submit_ans(request.form['coding_question_1_code'].strip(),users[username]['selected_questions']['c']['coding'][0]['id'],"c","coding",username,0)
        return redirect(url_for('quiz'))
    elif 'submit Q10' in request.form and coding_2_attempt < 3:
        submit_ans(request.form['coding_question_2_code'].strip(),users[username]['selected_questions']['py']['coding'][0]['id'],"py","coding",username,0)
        return redirect(url_for('quiz'))

    if (len(users[username]['attempts']['c']['choice'][0]['attempt_answer'])>=3):
        question_1_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['c']['choice'][0]['id'],"c","choice")
    if (len(users[username]['attempts']['c']['choice'][1]['attempt_answer'])>=3):
        question_2_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['c']['choice'][1]['id'],"c","choice")
    if (len(users[username]['attempts']['py']['choice'][0]['attempt_answer'])>=3):
        question_3_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['py']['choice'][0]['id'],"py","choice")
    if (len(users[username]['attempts']['py']['choice'][1]['attempt_answer'])>=3):
        question_4_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['py']['choice'][1]['id'],"py","choice")
    if (len(users[username]['attempts']['c']['multi-choice'][0]['attempt_answer'])>=3):
        question_5_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['c']['multi-choice'][0]['id'],"c","multi-choice")
    if (len(users[username]['attempts']['c']['multi-choice'][1]['attempt_answer'])>=3):
        question_6_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['c']['multi-choice'][1]['id'],"c","multi-choice")
    if (len(users[username]['attempts']['py']['multi-choice'][0]['attempt_answer'])>=3):
        question_7_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['py']['multi-choice'][0]['id'],"py","multi-choice")
    if (len(users[username]['attempts']['py']['multi-choice'][1]['attempt_answer'])>=3):
        question_8_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['py']['multi-choice'][1]['id'],"py","multi-choice")
    if (len(users[username]['attempts']['c']['coding'][0]['attempt_answer'])>=3):
        question_9_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['c']['coding'][0]['id'],"c","coding")
    if (len(users[username]['attempts']['py']['coding'][0]['attempt_answer'])>=3):
        question_10_ans = "The answer to this question is "+request_answer(users[username]['selected_questions']['py']['coding'][0]['id'],"py","coding")

    with open('users.json','r') as f:
        users=json.load(f)

    response = make_response(render_template('quiz.html', username=username, total_score=users[username]['total_score'], \
                                            single_choice_1_correct_ans= question_1_ans,\
                                            single_choice_2_correct_ans= question_2_ans,\
                                            single_choice_3_correct_ans= question_3_ans,\
                                            single_choice_4_correct_ans= question_4_ans,\
                                            multi_question_1_correct_ans= question_5_ans,\
                                            multi_question_2_correct_ans= question_6_ans,\
                                            multi_question_3_correct_ans= question_7_ans,\
                                            multi_question_4_correct_ans= question_8_ans,\
                                            coding_1_correct_ans= question_9_ans,\
                                            coding_2_correct_ans= question_10_ans,\
                                            choice_question_1=users[username]['selected_questions']['c']['choice'][0]['question'],\
                                            choice_1_attempt=len(users[username]['attempts']['c']['choice'][0]['attempt_answer']),\
                                            choice_1_ans=choice_1_ans,\
                                            choice_1_score=users[username]['attempts']['c']['choice'][0]['attempt_score'],\
                                            single_choice_1_a=users[username]['selected_questions']['c']['choice'][0]['choices'][0],\
                                            single_choice_1_b=users[username]['selected_questions']['c']['choice'][0]['choices'][1],\
                                            single_choice_1_c=users[username]['selected_questions']['c']['choice'][0]['choices'][2],\
                                            single_choice_1_d=users[username]['selected_questions']['c']['choice'][0]['choices'][3],\
                                            choice_question_2=users[username]['selected_questions']['c']['choice'][1]['question'],\
                                            choice_2_attempt=len(users[username]['attempts']['c']['choice'][1]['attempt_answer']),\
                                            choice_2_ans=choice_2_ans,\
                                            choice_2_score=users[username]['attempts']['c']['choice'][1]['attempt_score'],\
                                            single_choice_2_a=users[username]['selected_questions']['c']['choice'][1]['choices'][0],\
                                            single_choice_2_b=users[username]['selected_questions']['c']['choice'][1]['choices'][1],\
                                            single_choice_2_c=users[username]['selected_questions']['c']['choice'][1]['choices'][2],\
                                            single_choice_2_d=users[username]['selected_questions']['c']['choice'][1]['choices'][3],\
                                            choice_question_3=users[username]['selected_questions']['py']['choice'][0]['question'],\
                                            choice_3_attempt=len(users[username]['attempts']['py']['choice'][0]['attempt_answer']),\
                                            choice_3_ans=choice_3_ans,\
                                            choice_3_score=users[username]['attempts']['py']['choice'][0]['attempt_score'],\
                                            single_choice_3_a=users[username]['selected_questions']['py']['choice'][0]['choices'][0],\
                                            single_choice_3_b=users[username]['selected_questions']['py']['choice'][0]['choices'][1],\
                                            single_choice_3_c=users[username]['selected_questions']['py']['choice'][0]['choices'][2],\
                                            single_choice_3_d=users[username]['selected_questions']['py']['choice'][0]['choices'][3],\
                                            choice_question_4=users[username]['selected_questions']['py']['choice'][1]['question'],\
                                            choice_4_attempt=len(users[username]['attempts']['py']['choice'][1]['attempt_answer']),\
                                            choice_4_ans=choice_4_ans,\
                                            choice_4_score=users[username]['attempts']['py']['choice'][1]['attempt_score'],\
                                            single_choice_4_a=users[username]['selected_questions']['py']['choice'][1]['choices'][0],\
                                            single_choice_4_b=users[username]['selected_questions']['py']['choice'][1]['choices'][1],\
                                            single_choice_4_c=users[username]['selected_questions']['py']['choice'][1]['choices'][2],\
                                            single_choice_4_d=users[username]['selected_questions']['py']['choice'][1]['choices'][3],\
                                            multi_question_1=users[username]['selected_questions']['c']['multi-choice'][0]['question'],\
                                            multi_1_attempt=len(users[username]['attempts']['c']['multi-choice'][0]['attempt_answer']),\
                                            multi_1_ans=multi_1_ans,\
                                            multi_1_score=users[username]['attempts']['c']['multi-choice'][0]['attempt_score'],\
                                            multiple_choice_1_a=users[username]['selected_questions']['c']['multi-choice'][0]['choices'][0],\
                                            multiple_choice_1_b=users[username]['selected_questions']['c']['multi-choice'][0]['choices'][1],\
                                            multiple_choice_1_c=users[username]['selected_questions']['c']['multi-choice'][0]['choices'][2],\
                                            multiple_choice_1_d=users[username]['selected_questions']['c']['multi-choice'][0]['choices'][3],\
                                            multiple_choice_1_e=users[username]['selected_questions']['c']['multi-choice'][0]['choices'][4],\
                                            multi_question_2=users[username]['selected_questions']['c']['multi-choice'][1]['question'],\
                                            multi_2_attempt=len(users[username]['attempts']['c']['multi-choice'][1]['attempt_answer']),\
                                            multi_2_ans=multi_2_ans,\
                                            multi_2_score=users[username]['attempts']['c']['multi-choice'][1]['attempt_score'],\
                                            multiple_choice_2_a=users[username]['selected_questions']['c']['multi-choice'][1]['choices'][0],\
                                            multiple_choice_2_b=users[username]['selected_questions']['c']['multi-choice'][1]['choices'][1],\
                                            multiple_choice_2_c=users[username]['selected_questions']['c']['multi-choice'][1]['choices'][2],\
                                            multiple_choice_2_d=users[username]['selected_questions']['c']['multi-choice'][1]['choices'][3],\
                                            multiple_choice_2_e=users[username]['selected_questions']['c']['multi-choice'][1]['choices'][4],\
                                            multi_question_3=users[username]['selected_questions']['py']['multi-choice'][0]['question'],\
                                            multi_3_attempt=len(users[username]['attempts']['py']['multi-choice'][0]['attempt_answer']),\
                                            multi_3_ans=multi_3_ans,\
                                            multi_3_score=users[username]['attempts']['py']['multi-choice'][0]['attempt_score'],\
                                            multiple_choice_3_a=users[username]['selected_questions']['py']['multi-choice'][0]['choices'][0],\
                                            multiple_choice_3_b=users[username]['selected_questions']['py']['multi-choice'][0]['choices'][1],\
                                            multiple_choice_3_c=users[username]['selected_questions']['py']['multi-choice'][0]['choices'][2],\
                                            multiple_choice_3_d=users[username]['selected_questions']['py']['multi-choice'][0]['choices'][3],\
                                            multiple_choice_3_e=users[username]['selected_questions']['py']['multi-choice'][0]['choices'][4],\
                                            multi_question_4=users[username]['selected_questions']['py']['multi-choice'][1]['question'],\
                                            multi_4_attempt=len(users[username]['attempts']['py']['multi-choice'][1]['attempt_answer']),\
                                            multi_4_ans=multi_4_ans,\
                                            multi_4_score=users[username]['attempts']['py']['multi-choice'][1]['attempt_score'],\
                                            multiple_choice_4_a=users[username]['selected_questions']['py']['multi-choice'][1]['choices'][0],\
                                            multiple_choice_4_b=users[username]['selected_questions']['py']['multi-choice'][1]['choices'][1],\
                                            multiple_choice_4_c=users[username]['selected_questions']['py']['multi-choice'][1]['choices'][2],\
                                            multiple_choice_4_d=users[username]['selected_questions']['py']['multi-choice'][1]['choices'][3],\
                                            multiple_choice_4_e=users[username]['selected_questions']['py']['multi-choice'][1]['choices'][4],\
                                            coding_question_1=users[username]['selected_questions']['c']['coding'][0]['question'].split(' | ')[0],\
                                            coding_1_attempt=len(users[username]['attempts']['c']['coding'][0]['attempt_answer']),\
                                            coding_1_ans=coding_1_ans,\
                                            coding_question_1_code = users[username]['selected_questions']['c']['coding'][0]['question'].split(' | ')[1],\
                                            coding_1_score=users[username]['attempts']['c']['coding'][0]['attempt_score'],\
                                            coding_question_2=users[username]['selected_questions']['py']['coding'][0]['question'].split(' | ')[0],\
                                            coding_2_attempt=len(users[username]['attempts']['py']['coding'][0]['attempt_answer']),\
                                            coding_2_ans=coding_2_ans,\
                                            coding_question_2_code = users[username]['selected_questions']['py']['coding'][0]['question'].split(' | ')[1],\
                                            coding_2_score=users[username]['attempts']['py']['coding'][0]['attempt_score'],\
                                            ))
    #response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0'
    return response

if __name__ == '__main__':
    app.run(debug=True)
