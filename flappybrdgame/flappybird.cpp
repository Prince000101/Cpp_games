#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

const int width = 60, height = 20;
int birdY, pipeX, pipeGap, score, highScore = 0;
bool gameOver = false;

char birdSkin[3] = "@@"; // Yellow bird

// Pipe Colors
const string YELLOW = "\033[1;33m";
const string GREEN = "\033[1;32m";
const string RESET = "\033[0m";

// Safe clear screen (avoids -Wunused-result warning)
void clearScreen() {
#ifdef _WIN32
    (void)system("cls");
#else
    (void)system("clear");
#endif
}

// Non-blocking keyboard input
int kbhit() {
    struct termios oldt, newt;
    int ch, oldf;

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

void Draw() {
    clearScreen();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((x == 10 || x == 11) && (y == birdY || y == birdY + 1))
                cout << YELLOW << birdSkin << RESET;
            else if ((x >= pipeX && x <= pipeX + 6) && (y < pipeGap || y > pipeGap + 6))
                cout << GREEN << "██" << RESET;
            else
                cout << "  ";
        }
        cout << endl;
    }

    cout << "Score: " << score << " | High Score: " << highScore << endl;
}

void Input() {
    if (kbhit()) {
        char key = getch();
        if (key == ' ') birdY -= 3;
    }
}

void Logic() {
    birdY++;
    pipeX--;

    if (pipeX < -6) {
        pipeX = width - 1;
        pipeGap = rand() % (height - 10) + 3;
        score++;
        if (score > highScore) highScore = score;
    }

    usleep(70000); // Game speed

    if (birdY >= height - 1 || birdY < 0 ||
        ((pipeX <= 11 && pipeX + 6 >= 10) && (birdY < pipeGap || birdY + 1 > pipeGap + 6))) {
        gameOver = true;
    }
}

void ShowMenu() {
    char choice;
    while (true) {
        clearScreen();
        cout << "=========================" << endl;
        cout << "      FLAPPY BIRD       " << endl;
        cout << "=========================" << endl;
        cout << "  1. Start Game" << endl;
        cout << "  2. Change Bird" << endl;
        cout << "  3. Exit" << endl;
        cout << "=========================" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == '1') break;
        else if (choice == '2') {
            cout << "Choose Bird Style:\n1. @@ (Default)\n2. ^^ (Flappy)\n3. oO (Funny)\nChoice: ";
            int birdChoice;
            cin >> birdChoice;
            if (birdChoice == 1) strcpy(birdSkin, "@@");
            else if (birdChoice == 2) strcpy(birdSkin, "^^");
            else if (birdChoice == 3) strcpy(birdSkin, "oO");
        }
        else if (choice == '3') exit(0);
    }
}

void ResetGame() {
    birdY = height / 2;
    pipeX = width - 1;
    pipeGap = rand() % (height - 10) + 3;
    score = 0;
    gameOver = false;
}

int main() {
    srand(time(0));
    ShowMenu();

    while (true) {
        ResetGame();
        while (!gameOver) {
            Draw();
            Input();
            Logic();
        }

        cout << "Game Over! Your Score: " << score << endl;
        if (score > highScore) highScore = score;

        cout << "Play Again? (y/n): ";
        char choice;
        cin >> choice;
        if (choice != 'y') break;
    }

    cout << "Thanks for playing!" << endl;
    return 0;
}
