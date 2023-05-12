import socket
import time

# Set the server address and port number
SERVER_ADDRESS = ('127.0.0.1', 5001)

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

# Receive data and print
while True:
    data = conn.recv(1024)
    if not data:
        conn.send('data is empty'.encode('ascii'))
        break
    conn.send(('data received \n' + str(count)).encode('ascii'))
    time.sleep(0.5)
    message = data.decode('ascii')
    print("Received message:", message)
    if count == 5:
        break
    count += 1

# Close the connection
conn.close()
sock.close()
