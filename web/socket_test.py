import socket


def main():
    host = "192.168.4.1"  # Replace with your ESP32's IP address
    port = 81  # Same port as in the Arduino code

    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect to the ESP32 server
        client_socket.connect((host, port))

        # Send a message to the ESP32
        message = "Hello from Python!"
        client_socket.send(message.encode())

        while True:
            # Receive response from ESP32
            response = client_socket.recv(1024).decode()
            if response != "":
                print("Received:", response)

        # Close the socket
        client_socket.close()
    except Exception as e:
        print("Error:", e)


if __name__ == "__main__":
    main()
