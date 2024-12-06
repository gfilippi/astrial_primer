import socket
import sys
import threading

class TicTacToeClient:
    def __init__(self, host='localhost', port=33333):
        self.server_host = host
        self.server_port = port
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self):
        try:
            self.client_socket.connect((self.server_host, self.server_port))
            print(f"Connected to server at {self.server_host}:{self.server_port}")
        except Exception as e:
            print(f"Error connecting to server: {e}")
            sys.exit(1)

    def receive_messages(self):
        while True:
            try:
                message = self.client_socket.recv(1024).decode()
                if not message:
                    print("Disconnected from server.")
                    break
                print(message)
            except Exception as e:
                print(f"Error receiving message: {e}")
                break
        self.disconnect()

    def send_messages(self):
        try:
            while True:
                message = input()
                self.client_socket.sendall(message.encode())
        except (KeyboardInterrupt, EOFError):
            print("\nDisconnecting...")
            self.disconnect()

    def disconnect(self):
        try:
            self.client_socket.close()
        except Exception as e:
            print(f"Error closing socket: {e}")
        finally:
            sys.exit(0)

    def run(self):
        self.connect()

        # Start threads for sending and receiving messages
        threading.Thread(target=self.receive_messages, daemon=True).start()
        try:
            self.send_messages()
        except KeyboardInterrupt:
            print("Client shutting down...")

if __name__ == "__main__":
    client = TicTacToeClient()
    client.run()
