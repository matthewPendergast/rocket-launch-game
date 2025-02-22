#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "coordinates.h"
#include "path.h"
#include "rocket.h"
using namespace std;

void setCursor(Coordinates _coordinates);
void setupWindow(int _width, int _height);
void setupRockets(vector<Path*> _paths, int _startLine);
void setupPaths(int _startLine, int _finishLine, vector<Path*> _paths);
void setupPathNames(int _startLine, vector<Path*> _paths);
void showTitle();
void showIntro(Coordinates _messagePos);
Coordinates centerMessage(Coordinates _pos, string _message);
void displayMessage(Coordinates _pos, string _message);
Coordinates generateMovement(Coordinates _pos);
void eraseRocket(Coordinates _pos);
void displayRocket(Rocket& _rocket);
void displayTrail(Coordinates _pos);
void clearMessageBar(Coordinates _messagePos);

int main() {
    // Settings
    const int consoleWidth = 122;
    const int consoleHeight = 42;
    const int startLine = 30;
    const int finishLine = 3;
    Coordinates messagePos(1, 1);
    Coordinates endCoordinates(0, startLine + 4);
    // Game Variables/Instances
    bool gameOver = false;
    string endMessage;
    Rocket rocket1;
    Rocket rocket2;
    Rocket rocket3;
    Rocket rocket4;
    Rocket rocket5;
    Rocket rocket6;
    Path path1(1, 10, 20, rocket1, "USA");
    Path path2(21, 30, 40, rocket2, "UK");
    Path path3(41, 50, 60, rocket3, "China");
    Path path4(61, 70, 80, rocket4, "India");
    Path path5(81, 90, 100, rocket5, "Japan");
    Path path6(101, 110, 120, rocket6, "Russia");
    vector<Path*> paths = { &path1, &path2, &path3, &path4, &path5, &path6 };

    // Necessary for randomized game outcomes
    srand(time(0));

    // Initial setup
    setupWindow(consoleWidth, consoleHeight);
    setupRockets(paths, startLine);
    setupPaths(startLine, finishLine, paths);
    setupPathNames(startLine, paths);
    showTitle();
    showIntro(messagePos);

    // Main game loop
    while (!gameOver) {
        for (Path* path : paths) {
            Rocket& currRocket = *path->getRocket();
            // Stores coordinates
            Coordinates oldPos = currRocket.getCoordinates();
            // Generates and stores movement info
            Coordinates newPos = generateMovement(oldPos);
            // Ensures rocket is not displayed beyond finish line
            if (newPos.row <= finishLine) {
                newPos.row = finishLine;
            }
            // Updates coordinates
            currRocket.setCoordinates(newPos);
            // Updates rocket display
            eraseRocket(oldPos);
            displayRocket(currRocket);
            // Ensures trail is displayed within start and finish line
            // also prevents trail from overwriting side of rocket (_finishLine + 1)
            if (newPos.row >= finishLine + 1 and newPos.row < startLine - 3) {
                displayTrail(newPos);
            }
            // Win condition
            if (newPos.row <= finishLine) {
                // Ensures finish line isn't overwritten by rocket
                setCursor(newPos);
                cout << "-^";
                // Displays win message
                clearMessageBar(messagePos);
                endMessage = "Winner: " + path->getName() + "!";
                messagePos = centerMessage(messagePos, endMessage);
                displayMessage(messagePos, endMessage);
                gameOver = true;
                break;
            }
            // Collision condition
            if (newPos.col <= path->getLBnd() + 2 || newPos.col >= path->getRBnd() - 2) {
                // Show collision
                setCursor(newPos);
                cout << " X";
                // Displays collision message
                clearMessageBar(messagePos);
                endMessage = path->getName() + "'s rocket crashed!";
                messagePos = centerMessage(messagePos, endMessage);
                displayMessage(messagePos, endMessage);
                gameOver = true;
                break;
            }
            // Sets speed of rocket movement
            Sleep(15);
        }
    }
    // Prints file information below game
    setCursor(endCoordinates);
    return 0;
}

// Windows API Call, requires windows.h
// Source: https://learn.microsoft.com/en-us/windows/console/setconsolecursorposition
void setCursor(Coordinates _coordinates) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { _coordinates.col, _coordinates.row };
    SetConsoleCursorPosition(hConsole, coord);
}

// Source: https://cplusplus.com/forum/beginner/1481/
void setupWindow(int _width, int _height) {
    // Converts roughly to pixels, using static_cast for precision
    // int is a required parameter for MoveWindow()
    _width = (static_cast<float>(_width) * 8.18852);
    _height = (static_cast<float>(_height) * 17.31707);
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, _width, _height, TRUE);
}

void setupRockets(vector<Path*> _paths, int _startLine) {
    // Draws the rockets in their initial coordinatess
    for (Path* path : _paths) {
        Coordinates startCoordinates(path->getCntr(), _startLine - 3); // Pos(col, row)
        Rocket& currRocket = *path->getRocket();
        setCursor(startCoordinates);
        path->getRocket()->setCoordinates(startCoordinates);
        displayRocket(currRocket);
    }
}

void setupPaths(int _startLine, int _finishLine, vector<Path*> _paths) {
    for (int currRow = _startLine + 4; currRow >= _finishLine; --currRow) {
        // Draws the paths/boundaries
        for (int i = 0; i < _paths.size(); ++i) {
            Path* path = _paths[i];
            Coordinates pos(path->getLBnd(), currRow);
            setCursor(pos);
            cout << "|";
            // For final right-side border
            if (i == _paths.size() - 1) {
                int rightCol = path->getRBnd();
                pos.col = rightCol;
                setCursor(pos);
                cout << "|";
            }
            // For finish line
            if (currRow == _finishLine) {
                for (int j = 0; j < 122; ++j) {
                    pos.col = j;
                    setCursor(pos);
                    cout << "-";
                }
            }
            // For upper start line
            if (currRow == _startLine) {
                for (int k = 0; k < 122; ++k) {
                    pos.col = k;
                    setCursor(pos);
                    cout << "-";
                }
            }
            // For lower start line
            if (currRow == _startLine + 4) {
                for (int m = 0; m < 122; ++m) {
                    pos.col = m;
                    setCursor(pos);
                    cout << "-";
                }
            }
        }
    }
}

void setupPathNames(int _startLine, vector<Path*> _paths) {
    for (int i = 0; i < _paths.size(); ++i) {
        Path* path = _paths[i];
        string pathName = path->getName();
        Coordinates pos(path->getCntr(), _startLine + 2);
        // Centers the pathname
        pos.col -= pathName.length() / 2;
        pos.col++;
        setCursor(pos);
        cout << pathName;
    }
}

void showTitle() {
    // Displays game title
    Coordinates pos(61, 0);
    string gameName = "_-Rocket Launch Game-_";
    pos = centerMessage(pos, gameName);
    setCursor(pos);
    cout << gameName;
}

void showIntro(Coordinates _messagePos) {
    // Displays launch message
    string message;
    for (int i = 5; i >= 0; --i) {
        clearMessageBar(_messagePos);
        // Appends to message depending on countdown number
        if (i > 0) {
            message += to_string(i) + "... ";
        }
        else if (i == 0) {
            message += "Launch!";
        }
        // Centers message
        _messagePos = centerMessage(_messagePos, message);
        setCursor(_messagePos);
        cout << message;
        // Sets delay between countdown, skipped at 0
        if (i != 0) {
            Sleep(1000);
        }
    }
}

Coordinates centerMessage(Coordinates _pos, string _message) {
    _pos.col = (61 - (_message.length() / 2));
    // Fixes oddity in spacing when string is even-numbered
    if ((_message.length() % 2) == 0) {
        _pos.col += 1;
    }
    return _pos;
}

void displayMessage(Coordinates _pos, string _message) {
    setCursor(_pos);
    cout << _message;
}

Coordinates generateMovement(Coordinates _pos) {
    // Generates upward movement
    int jump = rand() % 3;
    _pos.row -= jump;
    // Random chance for side movement
    int leftRight = rand() % 5;
    if (leftRight == 0) {
        _pos.col += 1;
    }
    if (leftRight == 1) {
        _pos.col -= 1;
    }
    return _pos;
}

void eraseRocket(Coordinates _pos) {
    for (int i = 0; i < 3; ++i) {
        setCursor(_pos);
        cout << "   \n";
        _pos.row += 1;
    }
}

void displayRocket(Rocket& rocket) {
    vector<string> icon = rocket.getIcon();
    Coordinates pos = rocket.getCoordinates();
    for (int i = 0; i < icon.size(); ++i) {
        setCursor(pos);
        cout << icon[i];
        pos.row++;
    }
}

void displayTrail(Coordinates _pos) {
    // Sets trail below rocket
    _pos.row += 3;
    setCursor(_pos);
    cout << " .";
}


void clearMessageBar(Coordinates _messagePos) {
    for (int i = 0; i < 122; ++i) {
        _messagePos.col = i;
        setCursor(_messagePos);
        cout << " ";
    }
}