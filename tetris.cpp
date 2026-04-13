#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include <termios.h>
#include <algorithm>
#include <sys/select.h>
#include <unistd.h>


const int WIDTH = 10;
const int HEIGHT = 20;
int board[HEIGHT][WIDTH] = {0};
int pX, pY;

struct Point { int x, y; };
struct Piece {
    Point blocks[4];
    int color;
};

int getNextFromBag();
void spawnPiece();
bool checkCollision(int nextX, int nextY);
void rotatePiece();
void lockPiece();
void checkLines();
void draw();
bool kbhit();
void handleInput(bool &running);


Piece shapes[7] = {
    {{{0, 1}, {1, 1}, {2, 1}, {3, 1}}, 1}, // I
    {{{0, 0}, {0, 1}, {1, 1}, {2, 1}}, 2}, // J
    {{{2, 0}, {0, 1}, {1, 1}, {2, 1}}, 3}, // L
    {{{1, 0}, {2, 0}, {1, 1}, {2, 1}}, 4}, // O
    {{{1, 0}, {2, 0}, {0, 1}, {1, 1}}, 5}, // S
    {{{1, 0}, {0, 1}, {1, 1}, {2, 1}}, 6}, // T
    {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}, 7}  // Z
};

Piece currentPiece, nextPiece;
std::vector<int> bag;

// --- CORE LOGIC FUNCTIONS ---

int getNextFromBag() {
    if (bag.empty()) {
        bag = {0, 1, 2, 3, 4, 5, 6};
        
        static std::random_device rd; 
        static std::mt19937 g(rd()); 
        std::shuffle(bag.begin(), bag.end(), g);
    }
    int pieceID = bag.back();
    bag.pop_back();
    return pieceID;
}

void spawnPiece() {
    currentPiece = nextPiece;
    nextPiece = shapes[getNextFromBag()];
    pX = WIDTH / 2 - 2;
    pY = 0;
}

bool checkCollision(int nextX, int nextY) {
    for (int i = 0; i < 4; i++) {
        int targetX = nextX + currentPiece.blocks[i].x;
        int targetY = nextY + currentPiece.blocks[i].y;
        if (targetX < 0 || targetX >= WIDTH || targetY >= HEIGHT) return true;
        if (targetY >= 0 && board[targetY][targetX] != 0) return true;
    }
    return false;
}

void rotatePiece() {
    Piece oldPiece = currentPiece;
    for (int i = 0; i < 4; i++) {
        int temp = currentPiece.blocks[i].x;
        currentPiece.blocks[i].x = -currentPiece.blocks[i].y;
        currentPiece.blocks[i].y = temp;
    }
    if (checkCollision(pX, pY)) currentPiece = oldPiece;
}

void lockPiece() {
    for (int i = 0; i < 4; i++) {
        int tx = pX + currentPiece.blocks[i].x;
        int ty = pY + currentPiece.blocks[i].y;
        if (ty >= 0) board[ty][tx] = currentPiece.color;
    }
}

void checkLines() {
    for (int y = HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < WIDTH; x++) if (board[y][x] == 0) full = false;
        if (full) {
            for (int ty = y; ty > 0; ty--)
                for (int tx = 0; tx < WIDTH; tx++) board[ty][tx] = board[ty-1][tx];
            for (int tx = 0; tx < WIDTH; tx++) board[0][tx] = 0;
            y++; // Check same line again
        }
    }
}



void draw() {
    std::cout << "\033[H"; // Move cursor to top-left instead of system("clear") for smoothness
    for (int y = 0; y < HEIGHT; y++) {
        std::cout << "|";
        for (int x = 0; x < WIDTH; x++) {
            bool isActive = false;
            for (int i = 0; i < 4; i++) {
                if (x == pX + currentPiece.blocks[i].x && y == pY + currentPiece.blocks[i].y) 
                    isActive = true;
            }
            if (isActive || board[y][x] != 0) std::cout << "[]";
            else std::cout << "  ";
        }
        std::cout << "|\n";
    }
    std::cout << "----------\nControls: A, S, D, W (Rotate), Q (Quit)\n";
}



bool kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

void handleInput(bool &running) {
    if (kbhit()) {
        char key;
        if (read(0, &key, 1) > 0) {
            if (key == 'a' && !checkCollision(pX - 1, pY)) pX--;
            if (key == 'd' && !checkCollision(pX + 1, pY)) pX++;
            if (key == 's' && !checkCollision(pX, pY + 1)) pY++;
            if (key == 'w') rotatePiece();
            if (key == 'q') running = false;
        }
    }
}

// --- MAIN LOOP ---

int main() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    srand(time(0));
    nextPiece = shapes[getNextFromBag()];
    spawnPiece();

    bool gameRunning = true;
    auto lastTime = std::chrono::steady_clock::now();
    float timer = 0, delay = 0.5;

    while (gameRunning) {
        auto now = std::chrono::steady_clock::now();
        timer += std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        handleInput(gameRunning);

        if (timer > delay) {
            if (!checkCollision(pX, pY + 1)) pY++;
            else {
                lockPiece();
                checkLines();
                spawnPiece();
                if (checkCollision(pX, pY)) gameRunning = false;
            }
            timer = 0;
        }

        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore terminal
    std::cout << "\nGAME OVER!\n";
    return 0;
}