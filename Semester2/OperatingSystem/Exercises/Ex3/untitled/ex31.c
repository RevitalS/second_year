/******************************************
Student name: Dor Sharon
Student: 204239594
Course Exercise Group: 03
Exercise name :Exercise 3
******************************************/

#include <signal.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <sys/shm.h>

#define ROWS 8
#define COLS 8
#define SHM_SIZE 1024
#define FIFO_NAME "fifo_clientTOserver"

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3
#define UP_RIGHT 4
#define UP_LEFT 5
#define DOWN_RIGHT 6
#define DOWN_LEFT 7

#define BLACK '2'
#define WHITE '1'
#define EMPTY '0'

#define WHITE_WINS "GAME OVER\nWinning player: White\n"
#define BLACK_WINS "GAME OVER\nWinning player: Black\n"
#define NO_WINNER "GAME OVER\nNo winning player\n"

#define ERROR_FIFO "ERROR: FIFO creation failed.\n"
#define ERROR_SIGACTION "ERROR: Sigaction failed.\n"
#define ERROR_FTOK "ERROR: Key creation failed.\n"
#define ERROR_SHMGET "ERROR: Shared memory creation failed.\n"
#define ERROR_SHMAT "ERROR: Shared memory attachment failed.\n"
#define ERROR_SHMDT "ERROR: Shared memory detachment failed.\n"

/************************************************************************
* Function name: CreateNewGame                                          *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: resets the gameboard to the default starting  *
 * situation.                                                           *
************************************************************************/
void CreateNewGame();

/************************************************************************
* Function name: SetIndexAndInterval                                    *
* The Input: pointers to index and interval, coordinates of placement   *
 * and the direction.                                                   *
* The output: none                                                      *
* The Function operation: sets the index and interval according to the  *
 * direction and coordinates given.                                     *
************************************************************************/
void SetIndexAndInterval(int *index, int *indexInterval,
                         int x, int y, int direction);

/************************************************************************
* Function name: PlaceTokenOnBoard                                      *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols                                                              *
* The output: 1 if the placement is valid, 0 otherwise.                 *
* The Function operation: checks if the token placement is valid in     *
 * in all directions, and if so, makes the placement.                   *
************************************************************************/
int PlaceTokenOnBoard(int x, int y, char playerSymbol, char opponentSymbol);

/************************************************************************
* Function name: CountTokenConversions                                  *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols and the direction to count conversions in.                   *
* The output: number of conversions made in this direction.             *
* The Function operation: checks and counts how many token conversions  *
 * are possible in this direction.                                      *
************************************************************************/
int CountTokenConversions(int x, int y, char playerSymbol,
                          char opponentSymbol, int direction);

/************************************************************************
* Function name: TokenConversionIsPossible                              *
* The Input: coordinates of the token placement and the direction       *
 * to check                                                             *
* The output: 1 if a token conversion is possible in this direction,    *
 * 0 otherwise.                                                         *
* The Function operation: returns the result of the condition that      *
 * must be met in order for conversion to be possible in given direction*
************************************************************************/
int TokenConversionIsPossible(int x, int y, int direction);

/************************************************************************
* Function name: CheckingCondition                                      *
* The Input: index of the placement and the direction to check          *
* The output: 1 if the condition in this direction is met, 0 otherwise. *
* The Function operation: returns the result of the condition that must *
 * be met in this direction.                                            *
************************************************************************/
int CheckingCondition(int index, int direction);

/************************************************************************
* Function name: IsPlacementValid                                       *
* The Input: coordinates of the placement and the player's and          *
 * opponent's symbols.                                                  *
* The output: the number of conversions possible with this placement.   *
* The Function operation: places the token, counts the number of        *
 * conversions made as a result and then undoes that placement.         *
************************************************************************/
int IsPlacementValid(int x, int y, char playerSymbol, char opponentSymbol);

/************************************************************************
* Function name: IsGameOver                                             *
* The Input: the player's symbol                                        *
* The output: 1 if the white player won, 2 if the black player won,     *
 * 3 if it's a tie, 0 if the game isn't over                            *
* The Function operation: checks if the game is over, either beacuse    *
 * one of the players won or if it's a tie.                             *
************************************************************************/
int IsGameOver(char mySymbol);

/************************************************************************
* Function name: IsCoordinatesValid                                     *
* The Input: the input of the player                                    *
* The output: 1 if valid, 0 otherwise                                   *
* The Function operation: checks if the player's input is valid         *
************************************************************************/
int IsCoordinatesValid(char *input);

/************************************************************************
* Function name: ExitWithError                                          *
* The Input: an error message                                           *
* The output: none                                                      *
* The Function operation: Prints out the error message and              *
 * exits the program.                                                   *
************************************************************************/
void ExitWithError(char *errorMessage);

char gameBoard[ROWS * COLS];

/************************************************************************
* Function name: main                                                   *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: initializes the game board and sends signals *
 * to each player to read their moves.                                  *
************************************************************************/
int main(void) {
    pid_t clientPid1, clientPid2;
    key_t key;
    char *pShm;
    int shmid, fdFifo;

    // Create a new game board.
    CreateNewGame();

    // Create a new key for the shared memory.
    if ((key = ftok("ex31.c", 'k')) == -1) {
        ExitWithError(ERROR_FTOK);
    }

    // Create a new shared memory.
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        ExitWithError(ERROR_SHMGET);
    }

    // Attach to the shared memory.
    pShm = shmat(shmid, NULL, 0644);
    if (pShm == (char *) (-1)) {
        ExitWithError(ERROR_SHMAT);
    }

    // Make a new FIFO.
    if ((mkfifo(FIFO_NAME, 0666)) < 0) {
        ExitWithError(ERROR_FIFO);
    }

    // Open the FIFO.
    if ((fdFifo = open(FIFO_NAME, O_RDONLY)) < 0) {
        ExitWithError(ERROR_FIFO);
    }

    // Read the two PIDs of the client processes
    read(fdFifo, &clientPid1, sizeof(pid_t));
    read(fdFifo, &clientPid2, sizeof(pid_t));

    // Write "NONE" on the shared memory.
    strcpy(pShm, "_NONE");

    // Send SIGUSR1 to the first player.
    kill(clientPid1, SIGUSR1);

    // Wait until the first player play.
    while (strcmp(pShm + 1, "NONE") == 0) {
        sleep(1);
    }

    // Place the first player's token and update the board.
    PlaceTokenOnBoard((*(pShm + 2) - '0'), (*(pShm + 3) - '0'), BLACK, WHITE);

    // Send SIGUSR1 to the second player.
    kill(clientPid2, SIGUSR1);

    // Save the data that on the shared memory.
    char lastData[8];
    char player, opponent;
    strcpy(lastData, pShm + 1);

    // Update the game board.
    while (1) {
        // Wait until a player makes a move and then check which player it was.
        while (strcmp(pShm + 1, lastData) == 0);
        if (*(pShm + 1) == 'b') {
            player = BLACK;
            opponent = WHITE;
        } else if (*(pShm + 1) == 'w') {
            player = WHITE;
            opponent = BLACK;
        }

        // Place the player's token and update the board.
        PlaceTokenOnBoard((*(pShm + 2) - '0'), (*(pShm + 3) - '0'),
                          player, opponent);

        // Check if the game is over and announce a winner if so.
        if (IsGameOver(player)) {
            if (shmdt(pShm) == -1) {
                ExitWithError(ERROR_SHMDT);
            }

            // Close the FIFO and delete it.
            close(fdFifo);
            unlink(FIFO_NAME);
            return 0;
        }

        // Save the data on the shared memory.
        strcpy(lastData, pShm + 1);
    }
}

/************************************************************************
* Function name: CreateNewGame                                          *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: resets the gameboard to the default starting  *
 * situation.                                                           *
************************************************************************/
void CreateNewGame() {
    // Reset all the cell to 'O'.
    memset(gameBoard, EMPTY, sizeof(char) * ROWS * COLS);

    // Initialize the game with the default initial black and white cells.
    gameBoard[COLS * 3 + 3] = BLACK;
    gameBoard[COLS * 3 + 4] = WHITE;
    gameBoard[COLS * 4 + 3] = WHITE;
    gameBoard[COLS * 4 + 4] = BLACK;
}

/************************************************************************
* Function name: SetIndexAndInterval                                    *
* The Input: pointers to index and interval, coordinates of placement   *
 * and the direction.                                                   *
* The output: none                                                      *
* The Function operation: sets the index and interval according to the  *
 * direction and coordinates given.                                     *
************************************************************************/
void SetIndexAndInterval(int *index, int *indexInterval,
                         int x, int y, int direction) {
    switch (direction) {
        case UP: {
            *index = (y - 1) * COLS + x;
            *indexInterval = -COLS;
            break;
        }
        case DOWN: {
            *index = (y + 1) * COLS + x;
            *indexInterval = COLS;
            break;
        }
        case RIGHT: {
            *index = y * COLS + x + 1;
            *indexInterval = 1;
            break;
        }
        case LEFT: {
            *index = y * COLS + x - 1;
            *indexInterval = -1;
            break;
        }
        case UP_RIGHT: {
            *index = (y - 1) * COLS + x + 1;
            *indexInterval = -COLS + 1;
            break;
        }
        case UP_LEFT: {
            *index = (y - 1) * COLS + x - 1;
            *indexInterval = -COLS - 1;
            break;
        }
        case DOWN_RIGHT: {
            *index = (y + 1) * COLS + x + 1;
            *indexInterval = COLS + 1;
            break;
        }
        case DOWN_LEFT: {
            *index = (y + 1) * COLS + x - 1;
            *indexInterval = COLS - 1;
            break;
        }
        default:
            break;
    }
}

/************************************************************************
* Function name: PlaceTokenOnBoard                                      *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols                                                              *
* The output: 1 if the placement is valid, 0 otherwise.                 *
* The Function operation: checks if the token placement is valid in     *
 * in all directions, and if so, makes the placement.                   *
************************************************************************/
int PlaceTokenOnBoard(int x, int y, char playerSymbol,
                      char opponentSymbol) {
    int index = 0, indexInterval = 0, count, direction, i,
            totalConversions = 0;

    for (direction = UP; direction < DOWN_LEFT + 1; direction++) {
        SetIndexAndInterval(&index, &indexInterval, x, y, direction);
        count = CountTokenConversions(x, y, playerSymbol,
                                      opponentSymbol, direction);

        for (i = 0; i < count; i++) {
            gameBoard[index] = playerSymbol;
            index += indexInterval;
        }
        totalConversions += count;
    }

    if (totalConversions > 0) {
        // Once we've confirmed that the placement is valid,
        // we place the token.
        gameBoard[y * COLS + x] = playerSymbol;
        return 1;
    }

    // Otherwise, if the placement is invalid.
    return 0;
}

/************************************************************************
* Function name: CountTokenConversions                                  *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols and the direction to count conversions in.                   *
* The output: number of conversions made in this direction.             *
* The Function operation: checks and counts how many token conversions  *
 * are possible in this direction.                                      *
************************************************************************/
int CountTokenConversions(int x, int y, char playerSymbol,
                          char opponentSymbol, int direction) {

    int index, indexInterval, count = 0;

    SetIndexAndInterval(&index, &indexInterval, x, y, direction);
    if (TokenConversionIsPossible(x, y, direction)) {
        return 0;
    }

    // Count how many of the opponent's tokens can be converted
    // in this direction.
    while (CheckingCondition(index, direction) &&
           (gameBoard[index] == opponentSymbol)) {
        count++;
        index += indexInterval;
    }

    // Check if the place where we stopped has our symbol or opponent's.
    if (gameBoard[index] == playerSymbol) {
        return count;
    } else {
        return 0;
    }
}

/************************************************************************
* Function name: TokenConversionIsPossible                              *
* The Input: coordinates of the token placement and the direction       *
 * to check                                                             *
* The output: 1 if a token conversion is possible in this direction,    *
 * 0 otherwise.                                                         *
* The Function operation: returns the result of the condition that      *
 * must be met in order for conversion to be possible in given direction*
************************************************************************/
int TokenConversionIsPossible(int x, int y, int direction) {
    switch (direction) {
        case UP:
            return y % ROWS == 0 || y % ROWS == 1;
        case DOWN:
            return y % ROWS == ROWS || y % ROWS == ROWS - 1;
        case RIGHT:
            return x % COLS == COLS - 1 || x % COLS == COLS - 2;
        case LEFT:
            return x % COLS == 0 || x % COLS == 1;
        case UP_RIGHT:
            return y % ROWS == 0 || y % ROWS == 1 ||
                   x % COLS == COLS - 1 || x % COLS == COLS - 2;
        case UP_LEFT:
            return x % COLS == 0 || x % COLS == 1 ||
                   y % ROWS == 0 || y % ROWS == 1;
        case DOWN_RIGHT:
            return x % COLS == COLS - 1 || x % COLS == COLS - 2 ||
                   y % ROWS == ROWS - 2 || y % ROWS == ROWS - 1;
        case DOWN_LEFT:
            return x % COLS == 0 || x % COLS == 1 ||
                   y % ROWS == ROWS - 2 || y % ROWS == ROWS - 1;

        default:
            return 0;
    }
}

/************************************************************************
* Function name: CheckingCondition                                      *
* The Input: index of the placement and the direction to check          *
* The output: 1 if the condition in this direction is met, 0 otherwise. *
* The Function operation: returns the result of the condition that must *
 * be met in this direction.                                            *
************************************************************************/
int CheckingCondition(int index, int direction) {
    switch (direction) {
        case UP:
            return index > COLS;
        case DOWN:
            return index < ROWS * COLS;
        case RIGHT:
            return index % COLS != COLS - 1;
        case LEFT:
            return index % COLS != 0;
        case UP_RIGHT:
            return index % COLS != COLS - 1 && index > COLS;
        case UP_LEFT:
            return index % COLS != 0 && index > COLS;
        case DOWN_RIGHT:
            return index % COLS != COLS - 1 && index < ROWS * COLS;
        case DOWN_LEFT:
            return index % COLS != 0 && index < ROWS * COLS;

        default:
            return 0;
    }
}

/************************************************************************
* Function name: IsPlacementValid                                       *
* The Input: coordinates of the placement and the player's and          *
 * opponent's symbols.                                                  *
* The output: the number of conversions possible with this placement.   *
* The Function operation: places the token, counts the number of        *
 * conversions made as a result and then undoes that placement.         *
************************************************************************/
int IsPlacementValid(int x, int y, char playerSymbol, char opponentSymbol) {
    char tempBoard[ROWS * COLS];
    int i, j;

    // Copy the game board into a temporary variable.
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            tempBoard[i * COLS + j] = gameBoard[i * COLS + j];
        }
    }

    int result = PlaceTokenOnBoard(x, y, playerSymbol, opponentSymbol);

    // Restore the game board back.
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            gameBoard[i * COLS + j] = tempBoard[i * COLS + j];
        }
    }

    return result;

}

/************************************************************************
* Function name: IsGameOver                                             *
* The Input: the player's symbol                                        *
* The output: 1 if the white player won, 2 if the black player won,     *
 * 3 if it's a tie, 0 if the game isn't over                            *
* The Function operation: checks if the game is over, either beacuse    *
 * one of the players won or if it's a tie.                             *
************************************************************************/
int IsGameOver(char playerSymbol) {
    int y, x;
    char cellSymbol;
    int blackCount = 0, whiteCount = 0, boardIsNotFull = 0,
            blackHasValidPlacement = 0, whiteHasValidPlacement = 0;

    // Iterate through the game board cells
    for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLS; x++) {
            cellSymbol = gameBoard[y * COLS + x];

            // Count the black and white cells.
            if (cellSymbol == WHITE)
                whiteCount++;
            if (cellSymbol == BLACK)
                blackCount++;

            // If the cell is empty, check if the next player can place
            // their token there.
            if (cellSymbol == EMPTY) {
                boardIsNotFull = 1;
                if (IsPlacementValid(x, y, BLACK, WHITE) > 0) {
                    blackHasValidPlacement = 1;
                }
                if (IsPlacementValid(x, y, WHITE, BLACK) > 0) {
                    whiteHasValidPlacement = 1;
                }
            }

            if (blackCount && whiteCount && boardIsNotFull &&
                blackHasValidPlacement && whiteHasValidPlacement)
                break;
        }
        if (blackCount && whiteCount && boardIsNotFull &&
            blackHasValidPlacement && whiteHasValidPlacement)
            break;
    }

    // If there are no black cells at all, the white player wins.
    if (!blackCount) {
        write(STDOUT_FILENO, WHITE_WINS, strlen(WHITE_WINS));
        return 1;
    }

    // If there are no white cells at all, the black player wins.
    if (!whiteCount) {
        write(STDOUT_FILENO, BLACK_WINS, strlen(BLACK_WINS));
        return 2;
    }

    // If the next player can't move or the game board is full,
    // announce a winner accordingly.
    if (blackCount && whiteCount &&
        ((playerSymbol == WHITE && !blackHasValidPlacement) ||
         (playerSymbol == BLACK && !whiteHasValidPlacement) ||
         !boardIsNotFull)) {
        if (blackCount < whiteCount) {
            write(STDOUT_FILENO, WHITE_WINS, strlen(WHITE_WINS));
            return 1;
        } else if (blackCount > whiteCount) {
            write(STDOUT_FILENO, BLACK_WINS, strlen(BLACK_WINS));
            return 2;
        } else {
            write(STDOUT_FILENO, NO_WINNER, strlen(NO_WINNER));
            return 3;
        }
    }
    return 0;
}

/************************************************************************
* Function name: IsCoordinatesValid                                     *
* The Input: the input of the player                                    *
* The output: 1 if valid, 0 otherwise                                   *
* The Function operation: checks if the player's input is valid         *
************************************************************************/
int IsCoordinatesValid(char *input) {
    // Check if the coordinates input is valid and the number of the cell
    // coordinates is valid.
    return ((input[0] == '[') && (input[2] == ',') && (input[4] == ']') &&
            (input[1] >= 49) && (input[1] <= 56)
            && (input[3] >= 49) && (input[3] <= 56));
}

/************************************************************************
* Function name: ExitWithError                                          *
* The Input: an error message                                           *
* The output: none                                                      *
* The Function operation: Prints out the error message and              *
 * exits the program.                                                   *
************************************************************************/
void ExitWithError(char *errorMessage) {
    write(STDERR_FILENO, errorMessage, strlen(errorMessage));
    exit(1);
}