import os

INPUT_PIPE = "/tmp/input_pipe"
OUTPUT_PIPE = "/tmp/output_pipe"

def write_to_pipe(fd, data):
    os.write(fd, data.encode())

def read_from_pipe(fd):
    buffer = os.read(fd, 1024)
    return buffer.decode().rstrip('\0')

input_fd = os.open(INPUT_PIPE, os.O_WRONLY)
output_fd = os.open(OUTPUT_PIPE, os.O_RDONLY)

while True:
    # Send commands
    write_to_pipe(input_fd, "1|20.28.197.199|8081\n")  # Listen on 20.28.197.199:8080
    write_to_pipe(input_fd, "3|20.28.197.199|8081|Hello, Python!\n")  # Send message "Hello, Python!" to 20.28.197.199:8080
    write_to_pipe(input_fd, "4\n")  # Terminate child processes

    # Read responses
    response = read_from_pipe(output_fd)
    print("Response:", response)

os.close(input_fd)
os.close(output_fd)
