#include <Keypad.h>

// Розмір клавіатури
const byte ROWS = 4;
const byte COLS = 4;

// Схема кнопок клавіатури
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Піни клавіатури
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

// Ініціалізація клавіатури
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Ігрове поле
char board[3][3];
char currentPlayer = 'X';
bool gameActive = false;

// Тип гри
enum GameMode { MAN_VS_MAN, MAN_VS_AI, AI_VS_AI_RANDOM, AI_VS_AI_WIN };
GameMode currentMode = MAN_VS_MAN;

void setup() {
    Serial.begin(9600);
    resetBoard();
    Serial.println("Awaiting mode selection...");
}

void loop() {
    // Перевірка на команди з серійного порту
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        // Вибір режиму гри
        if (command.startsWith("MODE")) {
            int mode = command.substring(5).toInt();
            switch (mode) {
                case 1:
                    currentMode = MAN_VS_AI;
                    Serial.println("Mode: Man vs AI");
                    break;
                case 2:
                    currentMode = MAN_VS_MAN;
                    Serial.println("Mode: Man vs Man");
                    break;
                case 3:
                    currentMode = AI_VS_AI_RANDOM;
                    Serial.println("Mode: AI vs AI (Random)");
                    break;
                case 4:
                    currentMode = AI_VS_AI_WIN;
                    Serial.println("Mode: AI vs AI (Win Strategy)");
                    break;
                default:
                    Serial.println("Invalid mode!");
            }
            resetBoard();
            gameActive = true;
            Serial.println("Game started!");
            printBoard();
        } else if (command == "RESET") {
            resetBoard();
            gameActive = true;
            Serial.println("Game reset!");
        }
    }

    // Якщо гра не активна, чекати команди
    if (!gameActive) {
        return;
    }

    // Обробка режимів гри
    switch (currentMode) {
        case MAN_VS_MAN:
            handlePlayerInput();
            break;
        case MAN_VS_AI:
            if (currentPlayer == 'X') { // Гравець X
                handlePlayerInput();
            } else if (currentPlayer == 'O' && gameActive) { // Хід AI
                makeAIMoveRandom();
            }
            break;
        case AI_VS_AI_RANDOM:
            if (gameActive) makeAIMoveRandom();
            if (gameActive) makeAIMoveRandom();
            break;
        case AI_VS_AI_WIN:
            if (gameActive) makeAIWinMove();
            if (gameActive) makeAIWinMove();
            break;
    }
}

void handlePlayerInput() {
    char key = keypad.getKey();
    if (key && key >= '1' && key <= '9') {
        int pos = key - '1';
        int row = pos / 3;
        int col = pos % 3;

        if (board[row][col] == '-') {
            board[row][col] = currentPlayer;

            printBoard();

            if (checkWin()) {
                Serial.print("WIN ");
                Serial.println(currentPlayer);
                gameActive = false;
                return;
            }

            if (isBoardFull()) {
                Serial.println("DRAW");
                gameActive = false;
                return;
            }

            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        } else {
            Serial.println("Cell already taken!");
        }
    }
}

void makeAIMoveRandom() {
    int row, col;
    do {
        row = random(0, 3);
        col = random(0, 3);
    } while (board[row][col] != '-');

    board[row][col] = currentPlayer;

    printBoard();

    if (checkWin()) {
        Serial.print("WIN ");
        Serial.println(currentPlayer);
        gameActive = false;
        return;
    }

    if (isBoardFull()) {
        Serial.println("DRAW");
        gameActive = false;
        return;
    }

    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

void makeAIWinMove() {
    // Поки що виграшна стратегія не реалізована, викликаємо випадковий хід
    makeAIMoveRandom();
}

void printBoard() {
    Serial.println("BOARD");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Serial.print(board[i][j]);
            if (j < 2) Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println();
}

bool checkWin() {
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '-') ||
            (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '-')) {
            return true;
        }
    }
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '-') ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '-')) {
        return true;
    }
    return false;
}

bool isBoardFull() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '-') {
                return false;
            }
        }
    }
    return true;
}

void resetBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = '-';
        }
    }
    currentPlayer = 'X';
}
