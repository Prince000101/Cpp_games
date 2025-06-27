#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

#ifdef _WIN32
  #include <windows.h>
#endif

using namespace std;

// ANSI color codes
static constexpr char const* RESET   = "\033[0m";
static constexpr char const* RED     = "\033[31m";
static constexpr char const* GREEN   = "\033[32m";
static constexpr char const* YELLOW  = "\033[33m";
static constexpr char const* BLUE    = "\033[34m";
static constexpr char const* MAGENTA = "\033[35m";
static constexpr char const* CYAN    = "\033[36m";
static constexpr char const* WHITE   = "\033[37m";

// Clear the console screen
void clearScreen() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    COORD home = {0, 0};
    DWORD written;
    FillConsoleOutputCharacter(hOut, ' ', cells, home, &written);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cells, home, &written);
    SetConsoleCursorPosition(hOut, home);
#else
    cout << "\033[2J\033[H";
#endif
}

// Print centered banner
void printHeader() {
    const string title     = "TIC TAC TOE";
    const string colLabels = "   1   2   3";
    int totalWidth         = (int)colLabels.size();

    const string TL  = u8"╔", TR = u8"╗";
    const string BL  = u8"╚", BR = u8"╝";
    const string HOR = u8"═", VER = u8"║";

    int innerWidth = (int)title.size() + 2;
    int padLeft  = max(0, (totalWidth - innerWidth) / 2);

    cout << MAGENTA
         << string(padLeft, ' ') 
         << TL 
         << string(innerWidth, HOR[0]) 
         << TR 
         << "\n";

    cout << string(padLeft, ' ')
         << VER << ' ' << title << ' ' << VER
         << "\n";

    cout << string(padLeft, ' ')
         << BL
         << string(innerWidth, HOR[0])
         << BR
         << RESET
         << "\n\n";
}

// Draw the game board
void printBoard(const vector<char>& b) {
    clearScreen();
    printHeader();

    cout << "   " << BLUE << "1" << RESET;
    cout << "   " << BLUE << "2" << RESET;
    cout << "   " << BLUE << "3" << RESET << "\n";

    cout << "  " << MAGENTA << "┌───┬───┬───┐" << RESET << "\n";

    for (int r = 0; r < 3; ++r) {
        cout << YELLOW << char('A' + r) << " " << RESET << "│";
        for (int c = 0; c < 3; ++c) {
            char cell = b[r * 3 + c];
            cout << " " 
                 << (cell == ' ' 
                     ? WHITE + string(".") + RESET 
                     : (cell == 'X' 
                        ? GREEN + string("X") + RESET 
                        : RED   + string("O") + RESET))
                 << " ";
            cout << MAGENTA << "│" << RESET;
        }
        cout << "\n";
        if (r < 2) {
            cout << "  " << MAGENTA << "├───┼───┼───┤" << RESET << "\n";
        } else {
            cout << "  " << MAGENTA << "└───┴───┴───┘" << RESET << "\n";
        }
    }
}

// Check win conditions
bool checkWin(const vector<char>& b, char p) {
    for (int i = 0; i < 3; ++i) {
        if ((b[i*3] == p && b[i*3+1] == p && b[i*3+2] == p) ||
            (b[i]   == p && b[i+3]  == p && b[i+6]  == p))
            return true;
    }
    return (b[0]==p && b[4]==p && b[8]==p) ||
           (b[2]==p && b[4]==p && b[6]==p);
}

bool isBoardFull(const vector<char>& b) {
    return find(b.begin(), b.end(), ' ') == b.end();
}

int parseMove(const string& mv) {
    if (mv.size() != 2) return -1;
    char r = toupper(mv[0]), c = mv[1];
    if (r < 'A' || r > 'C' || c < '1' || c > '3') return -1;
    return (r - 'A') * 3 + (c - '1');
}

int findImmediate(const vector<char>& b, char p) {
    for (int i = 0; i < 9; ++i) {
        if (b[i] == ' ') {
            vector<char> tmp = b;
            tmp[i] = p;
            if (checkWin(tmp, p)) return i;
        }
    }
    return -1;
}

pair<int,int> minimax(vector<char>& b, char turn, char ai, char human, int depth = 0) {
    if      (checkWin(b, ai))    return { 10 - depth, -1 };
    else if (checkWin(b, human)) return {-10 + depth, -1 };
    else if (isBoardFull(b))     return {  0,          -1 };

    int bestScore = (turn == ai) ? -1000 : 1000;
    int bestMove  = -1;

    for (int i = 0; i < 9; ++i) {
        if (b[i] == ' ') {
            b[i] = turn;
            auto [sc, mv] = minimax(b, (turn==ai?human:ai), ai, human, depth+1);
            b[i] = ' ';
            if ((turn == ai && sc > bestScore) ||
                (turn != ai && sc < bestScore)) {
                bestScore = sc;
                bestMove  = i;
            }
        }
    }
    return {bestScore, bestMove};
}

void animateThinking() {
    static const char* art[] = {
        "       ___________       ",
        "      |.---------.|      ",
        "      || THINKING||      ",
        "      ||   ...   ||      ",
        "      |'---------'|      ",
        "       `)__ ____('       ",
        "       [=== -- o ]--.    ",
        "     __'---------'__ \\   ",
        "    [::::::::::: :::] )  ",
        "     `\"\"'\"\"\"\"'\"\"\"\"`/T\\   ",
        nullptr
    };
    clearScreen();
    printHeader();
    cout << YELLOW;
    for (auto p = art; *p; ++p) cout << *p << "\n";
    cout << RESET << flush;
    this_thread::sleep_for(chrono::milliseconds(1000));
}

void computerMove(vector<char>& b, char ai, char human) {
    animateThinking();

    if (int m = findImmediate(b, ai); m != -1) {
        b[m] = ai; return;
    }
    if (int m = findImmediate(b, human); m != -1) {
        b[m] = ai; return;
    }
    int empty = count(b.begin(), b.end(), ' ');
    if (empty <= 6) {
        auto [_, mv] = minimax(b, ai, ai, human);
        if (mv != -1) {
            b[mv] = ai; return;
        }
    }
    if (b[4] == ' ') { b[4] = ai; return; }
    for (int c : {0,2,6,8}) {
        if (b[c] == ' ') { b[c] = ai; return; }
    }
    for (int i = 0; i < 9; ++i) {
        if (b[i] == ' ') { b[i] = ai; return; }
    }
}

int main() {
    const char HUMAN    = 'X';
    const char COMPUTER = 'O';
    bool playAgain = true;

    while (playAgain) {
        vector<char> board(9, ' ');

        bool humanFirst = false;
        while (true) {
            clearScreen();
            printHeader();
            cout << "Do you want to go first? (y/n): ";
            char c;
            if (!(cin >> c)) return 0;
            c = tolower(c);
            if (c=='y' || c=='n') {
                humanFirst = (c=='y');
                break;
            }
            cout << RED << " Please type 'y' or 'n'." << RESET << "\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        char turn = humanFirst ? HUMAN : COMPUTER;
        while (true) {
            printBoard(board);

            if (turn == HUMAN) {
                int idx = -1;
                string mv;
                do {
                    cout << GREEN << "Your move (A1–C3): " << RESET;
                    if (!getline(cin, mv)) { cin.clear(); continue; }
                    idx = parseMove(mv);
                    if (idx<0 || board[idx]!=' ')
                        cout << RED << " Invalid or occupied. Try again.\n" << RESET;
                } while (idx<0 || board[idx]!=' ');
                board[idx] = HUMAN;

                if (checkWin(board, HUMAN)) {
                    printBoard(board);
                    cout << GREEN << "You win! 🎉\n" << RESET;
                    break;
                }
            } else {
                computerMove(board, COMPUTER, HUMAN);
                printBoard(board);
                cout << YELLOW << "Computer has moved.\n" << RESET;
                if (checkWin(board, COMPUTER)) {
                    cout << RED << "Computer wins! 😢\n" << RESET;
                    break;
                }
            }

            if (isBoardFull(board)) {
                printBoard(board);
                cout << YELLOW << "It's a draw!\n" << RESET;
                break;
            }

            turn = (turn==HUMAN ? COMPUTER : HUMAN);
        }

        while (true) {
            cout << "Play again? (y/n): ";
            char c;
            if (!(cin >> c)) return 0;
            c = tolower(c);
            if (c=='y' || c=='n') {
                playAgain = (c=='y');
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            cout << RED << " Please type 'y' or 'n'." << RESET << "\n";
        }
    }

    cout << CYAN << "Thanks for playing! Goodbye.\n" << RESET;
    return 0;
}
