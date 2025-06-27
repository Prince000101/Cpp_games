#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

// made by prince on batch 7-8 pm
#define MAX_ATTEMPTS 5

using namespace std;

// Function to clear the console screen
void clearScreen() {
#ifdef _WIN32
    (void)system("cls");  // For Windows
#else
    (void)system("clear");  // For Unix/Linux/Mac
#endif
}

// Function to print colored text
void printColor(const string& text, const string& colorCode) {
    cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

// Function to print game title with colors and borders
void printTitle(const string& title) {
    string border = string(title.size() + 6, '*');
    printColor(border, "33"); // Yellow color
    cout << endl;
    printColor("*  " + title + "  *", "36"); // Cyan color
    cout << endl;
    printColor(border, "33");
    cout << endl << endl;
}

// Main class for the Hangman game
class HangmanGame {
public:
    HangmanGame() {
        srand(static_cast<unsigned int>(time(nullptr)));
        secretWord = getRandomWord();
        currentWord = string(secretWord.length(), '-');
        attemptsLeft = MAX_ATTEMPTS;
    }

    void play() {
        clearScreen();
        printTitle("Welcome to Hangman game! .Made by Prince.");
        printColor(centerText("Category: Fruits"), "32");
        cout << endl;
        cout << centerText("You have " + to_string(attemptsLeft) + " attempts to guess the fruit name.") << endl;

        while (attemptsLeft > 0) {
            displayGameInfo();
            drawHangman(attemptsLeft);
            char guess;
            cout << centerText("Guess a letter: ");
            cin >> guess;

            clearScreen();

            if (isalpha(guess)) {
                guess = tolower(guess);
                if (alreadyGuessed(guess)) {
                    printColor(centerText("You've already guessed that letter."), "31");
                } else {
                    bool correctGuess = updateCurrentWord(guess);
                    if (correctGuess) {
                        printColor(centerText("Good guess!"), "32");
                        if (currentWord == secretWord) {
                            displayGameInfo();
                            drawHangman(attemptsLeft);
                            printColor(centerText("Congratulations! You guessed the word: " + secretWord), "32");
                            cout << "\n\n";
                            return;
                        }
                    } else {
                        printColor(centerText("Incorrect guess."), "31");
                        attemptsLeft--;
                    }
                }
            } else {
                printColor(centerText("Please enter a valid letter."), "31");
            }
        }

        displayGameInfo();
        drawHangman(attemptsLeft);
        printColor(centerText("You've run out of attempts. The word was: " + secretWord), "31");
        cout << "\n\n";
    }

private:
    string secretWord;
    string currentWord;
    int attemptsLeft;
    vector<char> guessedLetters;

    string centerText(const string& text) {
        const int width = 50;
        const string padding = "     ";
        return padding + string((width - text.size()) / 2, ' ') + text;
    }

    string getRandomWord() {
        vector<string> words = { "apple", "banana", "mango", "grape" ,"orange", "strawberry", "blueberry", "watermelon", "pineapple", "kiwi", "cherry", "peach", "pear", "plum", "lemon", };
        int index = rand() % words.size();
        return words[index];
    }

    bool alreadyGuessed(char letter) {
        return find(guessedLetters.begin(), guessedLetters.end(), letter) != guessedLetters.end();
    }

    bool updateCurrentWord(char letter) {
        bool correctGuess = false;
        for (int i = 0; i < secretWord.length(); i++) {
            if (secretWord[i] == letter) {
                currentWord[i] = letter;
                correctGuess = true;
            }
        }
        guessedLetters.push_back(letter);
        return correctGuess;
    }

    void displayGameInfo() {
        cout << centerText("Word: " + currentWord) << endl;
        cout << centerText("Attempts left: " + to_string(attemptsLeft)) << endl;
        cout << centerText("Guessed letters: ");
        for (char letter : guessedLetters) {
            cout << letter << " ";
        }
        cout << endl;
    }

    void drawHangman(int attemptsLeft) {
        cout << "     _____" << endl;
        cout << "     |   |" << endl;
        cout << "     |   O" << endl;
        if (attemptsLeft < 5) cout << "     |   |" << endl;
        if (attemptsLeft < 4) cout << "     |  /|\\" << endl;
        if (attemptsLeft < 3) cout << "     |   |  " << endl;
        if (attemptsLeft < 2) cout << "     |   |" << endl;
        if (attemptsLeft < 1) cout << "     |  / \\" << endl;
        cout << "     |" << endl;
        cout << "     |" << endl;
    }
};

// Main function
int main() {
    HangmanGame game;
    game.play();
    return 0;
}
