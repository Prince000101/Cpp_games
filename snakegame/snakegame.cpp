#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>

using namespace std;

// ANSI Colors
#define GREEN "\033[32m"   // Snake
#define WHITE "\033[37m"   // Borders
#define RED "\033[31m"     // Apple
#define YELLOW "\033[33m"  // Golden Apple
#define RESET "\033[0m"

const int width = 40, height = 20;
int snakeX, snakeY, fruitX, fruitY, score, highScore;
vector<pair<int, int>> snakeBody;
char direction;
bool gameOver = false, wrapMode = true;
int speed = 100000;
bool goldenApple = false;

// Keyboard Input Handling
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Read High Score from File
void LoadHighScore() {
    ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
}

// Save High Score to File
void SaveHighScore() {
    if (score > highScore) {
        highScore = score;
        ofstream file("highscore.txt");
        file << highScore;
        file.close();
    }
}

// Generate New Apple
void GenerateApple() {
    bool valid;
    do {
        valid = true;
        fruitX = rand() % (width - 2) + 1;
        fruitY = rand() % (height - 2) + 1;

        // Check if the apple spawns inside the snake
        for (auto &segment : snakeBody) {
            if (segment.first == fruitX && segment.second == fruitY) {
                valid = false;
                break;
            }
        }
        if (fruitX == snakeX && fruitY == snakeY) valid = false;

    } while (!valid);

    goldenApple = (rand() % 10 == 0); // 10% chance for golden apple
}

// Show Main Menu
void ShowMenu() {
    (void)system("clear");
    cout << WHITE << "===== SNAKE GAME =====\n" << RESET;
    cout << "1. Start Game\n";
    cout << "2. Exit\n";
    cout << "Enter your choice: ";
    char choice;
    cin >> choice;

    if (choice == '2') {
        cout << "Thanks for playing!\n";
        exit(0);
    }
}

// Select Border Mode
void SelectBorderMode() {
    (void)system("clear");
    cout << WHITE << "===== SELECT BORDER TYPE =====\n" << RESET;
    cout << "1. Portal Borders (Snake wraps around)\n";
    cout << "2. Normal Borders (Snake dies on walls)\n";
    cout << "Enter your choice: ";
    char choice;
    cin >> choice;

    wrapMode = (choice == '1');
}

// Draw Game Board with Colors
void Draw() {
    (void)system("clear");

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
                cout << WHITE << "█" << RESET; // Borders
            else if (x == snakeX && y == snakeY)
                cout << GREEN << "O" << RESET; // Snake head
            else if (x == fruitX && y == fruitY)
                cout << (goldenApple ? YELLOW : RED) << "F" << RESET; // Apple
            else {
                bool printed = false;
                for (auto &segment : snakeBody) {
                    if (segment.first == x && segment.second == y) {
                        cout << GREEN << "o" << RESET; // Snake body
                        printed = true;
                        break;
                    }
                }
                if (!printed) cout << " ";
            }
        }
        cout << endl;
    }

    cout << "Score: " << score << "   High Score: " << highScore << endl;
}

// Input Handling for Arrow Keys & WASD
void Input() {
    if (kbhit()) {
        char key = getch();
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A' && direction != 's') direction = 'w'; // Up
            else if (key == 'B' && direction != 'w') direction = 's'; // Down
            else if (key == 'C' && direction != 'a') direction = 'd'; // Right
            else if (key == 'D' && direction != 'd') direction = 'a'; // Left
        }
        else {
            if ((key == 'w' && direction != 's') ||
                (key == 'a' && direction != 'd') ||
                (key == 's' && direction != 'w') ||
                (key == 'd' && direction != 'a')) {
                direction = key;
            }
        }
    }
}

// Game Logic
void Logic() {
    int prevX = snakeX, prevY = snakeY;
    int prev2X, prev2Y;

    if (direction == 'w') snakeY--;
    else if (direction == 's') snakeY++;
    else if (direction == 'a') snakeX--;
    else if (direction == 'd') snakeX++;

    if (wrapMode) {
        if (snakeX <= 0) snakeX = width - 2;
        else if (snakeX >= width - 1) snakeX = 1;
        if (snakeY <= 0) snakeY = height - 2;
        else if (snakeY >= height - 1) snakeY = 1;
    } else {
        if (snakeX <= 0 || snakeX >= width - 1 || snakeY <= 0 || snakeY >= height - 1)
            gameOver = true;
    }

    for (auto &segment : snakeBody) {
        prev2X = segment.first;
        prev2Y = segment.second;
        segment.first = prevX;
        segment.second = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    for (auto &segment : snakeBody) {
        if (segment.first == snakeX && segment.second == snakeY) {
            gameOver = true;
        }
    }

    if (snakeX == fruitX && snakeY == fruitY) {
        score += goldenApple ? 5 : 1;
        snakeBody.push_back({prevX, prevY});
        GenerateApple();
    }

    usleep(speed);
}

// Main Function
int main() {
    srand(time(0));
    LoadHighScore();

    ShowMenu();
    SelectBorderMode();

    while (true) {
        snakeX = width / 2;
        snakeY = height / 2;
        direction = 'd';
        gameOver = false;
        snakeBody.clear();
        score = 0;

        GenerateApple(); // Generate initial apple

        while (!gameOver) {
            Draw();
            Input();
            Logic();
        }

        SaveHighScore();
        cout << "Game Over! Play Again? (y/n): ";
        char choice;
        cin >> choice;
        if (choice != 'y') break;
    }

    cout << "Thanks for playing!" << endl;
    return 0;
}
