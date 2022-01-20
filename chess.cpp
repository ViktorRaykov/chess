#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
using namespace std;

const int DEFAULT_TABLE_SIZE = 8;
const char EMPTY_CELL = '_';
const char PLAYER_KING = 'K';
const char PLAYER_ROOK_1 = '1';
const char PLAYER_ROOK_2 = '2';
const char CPU_KING = 'P';

struct point2D {
    int x;
    int y;
};

struct playerMove {
    char piece;
    int x;
    int y;
};

int getMenuChoice()
{
    int choice = 0;
    do {
        cout<<"1. Start"<<endl;
        cout<<"2. Change table size"<<endl;
        cout<<"3. Exit"<<endl;
        cout<<"Enter your choice:\n";
        cin>>choice;
    } while (choice < 1 or choice > 3);
    return choice;
}

int getNewTableSize() {
    int newSize = 0;
    do
    {
        cout << "Please enter new table size: (>=6)\n";
        cin >> newSize;
    } while (newSize < 6);
    return newSize;
}

char** generateTable(int size) {
    char** result = new char*[size];
    for (int i = 0; i < size; i++) {
        result[i] = new char[size];
        for (int j = 0; j < size; j++) {
            result[i][j] = EMPTY_CELL;
        }
    }
    return result;
}

void putAtRandomFreeCell(char** table, int size, char piece) {
    int x = 0;
    int y = 0;
    do {
        x = rand() % size;
        y = rand() % size;
    } while (table[y][x] != EMPTY_CELL);
    table[y][x] = piece;
}

void clearBoard(char** table, int size) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            table[y][x] = EMPTY_CELL;
        }
    }
}

point2D findPositionOnBoard(char** table, int size, char piece) {
    point2D result = {0,0};
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (table[y][x] == piece) {
                result.x = x;
                result.y = y;
                return result;
            }
        }
    }
    return result;
}

bool areAdjacent(point2D a, point2D b) {
    return (abs(a.x - b.x) <= 1) and (abs(a.y - b.y) <= 1);
}

bool isReachableByRooks(char** table, int size, point2D cell) {
    // Going up
    for (int y = cell.y - 1; y >= 0; y--) {
        if (table[y][cell.x] == PLAYER_ROOK_1 or table[y][cell.x] == PLAYER_ROOK_2) {
            return true;
        }
        else if (table[y][cell.x] == EMPTY_CELL) {
            continue;
        }
        else {
            break;
        }
    }
    // Going down
    for (int y = cell.y + 1; y < size; y++) {
        if (table[y][cell.x] == PLAYER_ROOK_1 or table[y][cell.x] == PLAYER_ROOK_2) {
            return true;
        }
        else if (table[y][cell.x] == EMPTY_CELL) {
            continue;
        }
        else {
            break;
        }
    }
    // Going left
    for (int x = cell.x - 1; x >= 0; x--) {
        if (table[cell.y][x] == PLAYER_ROOK_1 or table[cell.y][x] == PLAYER_ROOK_2) {
            return true;
        }
        else if (table[cell.y][x] == EMPTY_CELL) {
            continue;
        }
        else {
            break;
        }
    }
    // Going right
    for (int x = cell.x + 1; x < size ; x++) {
        if (table[cell.y][x] == PLAYER_ROOK_1 or table[cell.y][x] == PLAYER_ROOK_2) {
            return true;
        }
        else if (table[cell.y][x] == EMPTY_CELL) {
            continue;
        }
        else {
            break;
        }
    }

    return false;
}

bool cpuInCheckmate(char** table, int size) {
    point2D cpuKing = findPositionOnBoard(table, size, CPU_KING);
    point2D playerKing = findPositionOnBoard(table, size, PLAYER_KING);
    return areAdjacent(cpuKing, playerKing) or isReachableByRooks(table, size, cpuKing);
}

void printTable(char** table, int size) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cout<<table[y][x]<<" ";
        }
        cout<<endl;
        cout<<endl;
    }
}

void fillTableWithPieces(char** table, int size) {
    do {
        clearBoard(table, size);
        putAtRandomFreeCell(table, size, PLAYER_KING);
        putAtRandomFreeCell(table, size, PLAYER_ROOK_1);
        putAtRandomFreeCell(table, size, PLAYER_ROOK_2);
        putAtRandomFreeCell(table, size, CPU_KING);
    } while (cpuInCheckmate(table, size));
}

// if all cells (x, yMin+1), (x, yMin+2) ... (x, yMax-1) are free
bool AreAllEmptyInCol(char** table, int x, int yMin, int yMax) {
    for (int y = yMin + 1; y < yMax; y++) {
        if (table[y][x] != EMPTY_CELL) {
            return false;
        }
    }
    return true;
}

// if all cells (xMin+1, y), (xMin+2, y) ... (xMax-1, y) are free
bool AreAllEmptyInRow(char** table, int y, int xMin, int xMax) {
    for (int x = xMin + 1; x < xMax; x++) {
        if (table[y][x] != EMPTY_CELL) {
            return false;
        }
    }
    return true;
}

bool isValidMove(char** table, int size, playerMove move) {
    if (move.piece != PLAYER_KING and move.piece != PLAYER_ROOK_1 and move.piece != PLAYER_ROOK_2) {
        return false;
    }
    if (move.x < 0 or move.x >= size or move.y < 0 or move.y >= size) {
        return false;
    }
    if (table[move.y][move.x] != EMPTY_CELL) { // Only ally pieces block us, and we can't take any enemy pieces.
        return false;
    }
    if (move.piece == PLAYER_KING) {
        point2D ourKingPos = findPositionOnBoard(table, size, PLAYER_KING);
        if (areAdjacent({move.x, move.y}, ourKingPos)) {

            // if the enemy king can take us after our move.
            if (areAdjacent({move.x, move.y}, findPositionOnBoard(table, size, CPU_KING))) { 
                return false;
            }
            return true;
        }
        else {
            return false;
        }
    }
    else {
        point2D rookPos = findPositionOnBoard(table, size, move.piece);
        // 0 matching coordinates -> not ok
        // 1 matching -> ok
        // 2 matching -> same cell -> not ok
        if (rookPos.x == move.x ^ rookPos.y == move.y) {
            // ako sa na edna kolona
            if (rookPos.x == move.x) {
                return AreAllEmptyInCol(table, move.x, min(move.y, rookPos.y), max(move.y, rookPos.y));
            }
            else { // ako sa na v edin red
                return AreAllEmptyInRow(table, move.y, min(move.x, rookPos.x), max(move.x, rookPos.x));
            }
        }
        return false;
    }
}

void makeMove(char** table, int size, playerMove move) {
    point2D prevPos = findPositionOnBoard(table, size, move.piece);
    table[prevPos.y][prevPos.x] = EMPTY_CELL;
    table[move.y][move.x] = move.piece;
}

void startGame(int tableSize) {
    char** table = generateTable(tableSize);
    fillTableWithPieces(table, tableSize);
    do {
        printTable(table, tableSize);
        playerMove currentMove;
        do {
            cout<<"Enter your move: (<piece> <x> <y>)"<<endl;
            cin>>currentMove.piece>>currentMove.x>>currentMove.y;
        } while (!isValidMove(table, tableSize, currentMove));
        // We are sure the move is valid;
        makeMove(table, tableSize, currentMove);
        // TODO - move king
    } while (!cpuInCheckmate(table, tableSize));
}

int main()
{
    srand(time(0));
    int currentTableSize = DEFAULT_TABLE_SIZE;
    int choice = 0;
    do
    {
        choice = getMenuChoice();
        switch (choice)
        {
        case 1:
            startGame(currentTableSize);
            break;
        case 2:
            currentTableSize = getNewTableSize();
            break;
        case 3:
            return 0;
        }
    } while (choice != 3);
}
