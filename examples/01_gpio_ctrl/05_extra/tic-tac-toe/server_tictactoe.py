import socket
import threading
import sys
import random

# Game constants
BOARD_SIZE = 4
EMPTY = '.'

class TicTacToeServer:
    def __init__(self, host='localhost', port=12345):
        self.host = host
        self.port = port
        self.board = [[EMPTY] * BOARD_SIZE for _ in range(BOARD_SIZE)]
        self.players = {}
        self.current_turn = None
        self.lock = threading.Lock()
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.playing_against_computer = False
        self.client_sockets = []

    def reset_board(self):
        self.board = [[EMPTY] * BOARD_SIZE for _ in range(BOARD_SIZE)]
        self.players = {}
        self.current_turn = None
        self.playing_against_computer = False

    def print_board(self):
        for row in self.board:
            print(' '.join(row))
        print()

    def check_winner(self):
        # Check rows and columns
        for i in range(BOARD_SIZE):
            if self.board[i].count(self.board[i][0]) == BOARD_SIZE and self.board[i][0] != EMPTY:
                return self.board[i][0]
            if all(self.board[j][i] == self.board[0][i] and self.board[0][i] != EMPTY for j in range(BOARD_SIZE)):
                return self.board[0][i]

        # Check diagonals
        if all(self.board[i][i] == self.board[0][0] and self.board[0][0] != EMPTY for i in range(BOARD_SIZE)):
            return self.board[0][0]
        if all(self.board[i][BOARD_SIZE - i - 1] == self.board[0][BOARD_SIZE - 1] and self.board[0][BOARD_SIZE - 1] != EMPTY for i in range(BOARD_SIZE)):
            return self.board[0][BOARD_SIZE - 1]

        return None

    def is_draw(self):
        return all(cell != EMPTY for row in self.board for cell in row)

    def computer_move(self):
        empty_positions = [(x, y) for x in range(BOARD_SIZE) for y in range(BOARD_SIZE) if self.board[x][y] == EMPTY]
        if empty_positions:
            x, y = random.choice(empty_positions)
            self.board[x][y] = self.players['server']
            print(f"Server placed at ({x}, {y})")
            self.print_board()

    def disconnect_all_clients(self, message="Game over. Disconnecting.\n"):
        for sock in self.client_sockets:
            try:
                sock.sendall(message.encode())
                sock.close()
            except:
                pass
        self.client_sockets.clear()
        self.reset_board()

    def handle_client(self, client_socket, client_address):
        self.client_sockets.append(client_socket)
        try:
            client_socket.sendall(b"Welcome to Tic Tac Toe. Type '1' to play against another player, '2' to play against the computer:\n")
            choice = client_socket.recv(1024).decode().strip()

            if choice == '2':
                self.playing_against_computer = True
                client_socket.sendall(b"You chose to play against the server. Send your name to start:\n")
                client_name = client_socket.recv(1024).decode().strip()

                with self.lock:
                    self.players[client_name] = 'X'
                    self.players['server'] = 'O'
                    self.current_turn = client_name

                client_socket.sendall(f"Your symbol is {self.players[client_name]}\n".encode())
                self.play_game_against_computer(client_socket, client_name)
            elif choice == '1':
                self.handle_multiplayer(client_socket)
            else:
                client_socket.sendall(b"Invalid choice. Disconnecting.\n")
                self.disconnect_all_clients()
        except Exception as e:
            print(f"Client {client_address} disconnected or caused error: {e}")
        finally:
            self.disconnect_all_clients()

    def play_game_against_computer(self, client_socket, client_name):
        while True:
            try:
                if self.current_turn == client_name:
                    client_socket.sendall(b"Enter your move as x y (e.g., 1 1):\n")
                    move = client_socket.recv(1024).decode().strip()
                    try:
                        x, y = map(int, move.split())
                        if not (0 <= x < BOARD_SIZE and 0 <= y < BOARD_SIZE):
                            raise ValueError

                        with self.lock:
                            if self.board[x][y] != EMPTY:
                                client_socket.sendall(b"Error: Invalid position.\n")
                                continue
                            self.board[x][y] = self.players[client_name]
                            self.print_board()

                            winner = self.check_winner()
                            if winner:
                                print(f"THE WINNER IS {'server' if winner == 'O' else client_name}")
                                self.disconnect_all_clients()
                                return
                            elif self.is_draw():
                                print("GAME OVER")
                                self.disconnect_all_clients()
                                return

                            self.current_turn = 'server'

                    except (ValueError, IndexError):
                        client_socket.sendall(b"Error: Invalid input.\n")
                elif self.current_turn == 'server':
                    self.computer_move()
                    winner = self.check_winner()
                    if winner:
                        print(f"THE WINNER IS {'server' if winner == 'O' else client_name}")
                        self.disconnect_all_clients()
                        return
                    elif self.is_draw():
                        print("GAME OVER")
                        self.disconnect_all_clients()
                        return
                    self.current_turn = client_name
            except (ConnectionResetError, ConnectionAbortedError):
                print(f"Client {client_name} disconnected.")
                self.disconnect_all_clients()
                return

    def handle_multiplayer(self, client_socket):
        client_socket.sendall(b"Send your name to start:\n")
        client_name = client_socket.recv(1024).decode().strip()

        with self.lock:
            if len(self.players) >= 2:
                client_socket.sendall(b"Error: Game is full.\n")
                client_socket.close()
                return
            elif client_name in self.players:
                client_socket.sendall(b"Error: Name already taken.\n")
                client_socket.close()
                return

            self.players[client_name] = 'X' if not self.players else 'O'
            if not self.current_turn:
                self.current_turn = client_name

        client_socket.sendall(f"Your symbol is {self.players[client_name]}\n".encode())

        while True:
            try:
                with self.lock:
                    current_player = self.current_turn
                    current_socket = next(sock for name, sock in zip(self.players, self.client_sockets) if name == current_player)

                if current_player != client_name:
                    client_socket.sendall(b"Waiting for other player...\n")
                    continue

                client_socket.sendall(b"Enter your move as x y (e.g., 1 1):\n")
                move = client_socket.recv(1024).decode().strip()
                try:
                    x, y = map(int, move.split())
                    if not (0 <= x < BOARD_SIZE and 0 <= y < BOARD_SIZE):
                        raise ValueError

                    with self.lock:
                        if self.board[x][y] != EMPTY:
                            client_socket.sendall(b"Error: Invalid position.\n")
                            continue
                        self.board[x][y] = self.players[client_name]
                        self.print_board()

                        winner = self.check_winner()
                        if winner:
                            print(f"THE WINNER IS {current_player}")
                            self.disconnect_all_clients()
                            return
                        elif self.is_draw():
                            print("GAME OVER")
                            self.disconnect_all_clients()
                            return

                        self.current_turn = next(name for name in self.players if name != client_name)

                except (ValueError, IndexError):
                    client_socket.sendall(b"Error: Invalid input.\n")
            except (ConnectionResetError, ConnectionAbortedError):
                print(f"Client {client_name} disconnected.")
                self.disconnect_all_clients()
                return

    def run(self):
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(2)
        print(f"Server running on {self.host}:{self.port}")

        try:
            while True:
                client_socket, client_address = self.server_socket.accept()
                print(f"Connection from {client_address}")
                threading.Thread(target=self.handle_client, args=(client_socket, client_address)).start()
        except KeyboardInterrupt:
            print("Server shutting down...")
            self.disconnect_all_clients()
        finally:
            self.server_socket.close()

if __name__ == "__main__":
    TicTacToeServer().run()
