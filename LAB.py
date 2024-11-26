import threading
import time

import serial

def send_command(command):
    ser.write((command + "\n").encode())
    time.sleep(0.1)


def start_game(mode):
    print(f"Sending START command for MODE {mode}...")
    send_command(f"MODE {mode}")
    send_command("START")


def reset_game():
    print("Resetting game...")
    send_command("RESET")


def read_moves():
    print("Reading moves from Arduino...")
    while True:
        try:
            move = ser.readline().decode().strip()
            if move:
                print(move)
        except Exception as e:
            print(f"Error reading data: {e}")
            break


def main():
    global ser
    try:
        ser = serial.Serial("COM2", 9600, timeout=1)
        time.sleep(2)
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return

    print("Welcome to Tic-Tac-Toe!")
    print("Select a game mode:")
    print("1. Player vs AI")
    print("2. Player vs Player")
    print("3. AI vs AI (Random)")
    print("4. AI vs AI (Win Strategy)")
    print("5. Reset Game")

    reader_thread = threading.Thread(target=read_moves, daemon=True)
    reader_thread.start()

    while True:
        option = input("Choose an option (1-5): ").strip()
        if option == "1":
            start_game("1")
        elif option == "2":
            start_game("2")
        elif option == "3":
            start_game("3")
        elif option == "4":
            start_game("4")
        elif option == "5":
            reset_game()


if __name__ == "__main__":
    main()
