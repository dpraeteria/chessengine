#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "defs.h"
#include "data.h"
#include "protos.h"

#define SIZE 64
//side = LIGHT;
//xside = DARK;

// Function prototypes


void FENToPiece(const char* fen) {
    int index = 0;
    for (int i = 0; i < 64; i++) {
        piece[i] = 6;
    }

    for (int i = 0; fen[i] != '\0' && index < SIZE; i++) {
        char c = fen[i];
        if (c >= '1' && c <= '8') {
            index += c - '0'; // Skip empty squares
        }
        else if (c == '/') {
            continue; // Skip row separator
        }
        else {
            switch (c) {
            case 'p': piece[index++] = 0; break; // Pawn (black)
            case 'r': piece[index++] = 3; break; // Rook (black)
            case 'n': piece[index++] = 1; break; // Knight (black)
            case 'b': piece[index++] = 2; break; // Bishop (black)
            case 'q': piece[index++] = 4; break; // Queen (black)
            case 'k': piece[index++] = 5; break; // King (black)
            case 'P': piece[index++] = 0; break; // Pawn (white)
            case 'R': piece[index++] = 3; break; // Rook (white)
            case 'N': piece[index++] = 1; break; // Knight (white)
            case 'B': piece[index++] = 2; break; // Bishop (white)
            case 'Q': piece[index++] = 4; break; // Queen (white)
            case 'K': piece[index++] = 5; break; // King (white)
            }
        }
    }
}

void FENToColor(const char* fen) {
    int index = 0;
    for (int i = 0; i < 64; i++) {
        color[i] = 6;
    }

    for (int i = 0; fen[i] != '\0' && index < SIZE; i++) {
        char c = fen[i];
        if (c >= '1' && c <= '8') {
            index += c - '0'; // Skip empty squares
        }
        else if (c == '/') {
            continue; // Skip row separator
        }
        else {
            if (c >= 'a' && c <= 'z') {
                color[index++] = 1; // White pieces
            }
            else if (c >= 'A' && c <= 'Z') {
                color[index++] = 0; // Black pieces
            }
        }
    }

    // Fill remaining squares with empty color
    for (int i = index; i < SIZE; i++) {
        color[i] = 6; // Empty square
    }
}

void FENToBoard(const char* fen) {
    FENToColor(fen);
    FENToPiece(fen);
}


void print_board()
{
    int i;

    printf("\n8 ");
    for (i = 0; i < 64; ++i) {
        switch (color[i]) {
        case EMPTY:
            printf(" .");
            break;
        case LIGHT:
            printf(" %c", piece_char[piece[i]]);
            break;
        case DARK:
            printf(" %c", piece_char[piece[i]] + ('a' - 'A'));
            break;
        }
        if ((i + 1) % 8 == 0 && i != 63)
            printf("\n%d ", 7 - ROW(i));
    }
    printf("\n\n   a b c d e f g h\n\n");
}
