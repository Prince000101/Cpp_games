#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>

using namespace std;

// ANSI color codes
const string RESET  = "\033[0m";
const string RED    = "\033[31m";
const string GREEN  = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE   = "\033[34m";
const string CYAN   = "\033[36m";

// Clear screen function
void clearScreen() {
#ifdef _WIN32
    (void)system("cls");
#else
    (void)system("clear");
#endif
}

// Print the current game board
void printBoard(const vector<char>& board) {
    clearScreen();
    cout << GREEN << "    1   2   3" << RESET << endl;
    for (int i = 0; i < 3; ++i) {
        char rowLabel = 'A' + i;
        cout << YELLOW << " " << rowLabel << RESET << " ";
        for (int j = 0; j < 3; ++j) {
            cout << " " << (board[i * 3 + j] == ' ' ? '.' : board[i * 3 + j]) << " ";
            if (j < 2) cout << "|";
        }
        cout << endl;
        if (i < 2) {
            cout << "   ---|---|---" << endl;
        }
    }
}

// Check if a player has won
bool checkWin(const vector<char>& board, char player) {
    for (int i = 0; i < 3; ++i) {
        if ((board[i * 3] == player && board[i * 3 + 1] == player && board[i * 3 + 2] == player) ||
            (board[i] == player && board[i + 3] == player && board[i + 6] == player)) {
            return true;
        }
    }
    return (board[0] == player && board[4] == player && board[8] == player) ||
           (board[2] == player && board[4] == player && board[6] == player);
}

// Check if the board is full (draw)
bool isBoardFull(const vector<char>& board) {
    for (char cell : board) {
        if (cell == ' ') return false;
    }
    return true;
}

// Parse user input (e.g., A1 → index 0)
int parseMove(const string& move) {
    if (move.length() != 2) return -1;
    char rowChar = toupper(move[0]);
    char colChar = move[1];

    if (rowChar < 'A' || rowChar > 'C') return -1;
    if (colChar < '1' || colChar > '3') return -1;

    int row = rowChar - 'A';
    int col = colChar - '1';
    return row * 3 + col;
}

// Main game loop
int main() {
    vector<char> board(9, ' ');
    char currentPlayer = 'X';
    string moveStr;

    cout << CYAN << "Welcome to Tic-Tac-Toe!" << RESET << endl;

    while (true) {
        printBoard(board);
        cout << GREEN << "\nPlayer " << currentPlayer << ", enter your move (e.g., A1): " << RESET;
        cin >> moveStr;

        int move = parseMove(moveStr);

        if (move == -1 || board[move] != ' ') {
            cout << RED << "Invalid move. Press Enter to continue." << RESET << endl;
            cin.ignore();
            cin.get();
            continue;
        }

        board[move] = currentPlayer;

        if (checkWin(board, currentPlayer)) {
            printBoard(board);
            cout << GREEN << "\nPlayer " << currentPlayer << " wins!" << RESET << endl;
            break;
        }

        if (isBoardFull(board)) {
            printBoard(board);
            cout << YELLOW << "\nIt's a draw!" << RESET << endl;
            break;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }

    cout << CYAN << "\nThanks for playing!" << RESET << endl;
    return 0;
}
