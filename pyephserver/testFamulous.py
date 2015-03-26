import socket
import sys

HOST, PORT = "ec2-54-227-4-154.compute-1.amazonaws.com", 3000
data = " ".join(sys.argv[1:])

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(data + "\n")

    # Receive data from the server and shut down
    received = sock.recv(256*1024*1024*1024)
finally:
    sock.close()

print "Sent:     {}".format(data)
print "Received: {}".format(len(received))
