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

#define WHITE_WINS "Winning player: White\n"
#define BLACK_WINS "Winning player: Black\n"
#define NO_WINNER "No winning player\n"
#define ENTER_COORDINATES "Please choose a square\n"
#define NO_SUCH_SQUARE "No such square\nPlease choose another square\n"
#define INVALID_SQUARE "This square is invalid\nPlease choose another square\n"
#define WAITING_FOR_PLAYER "Waiting for the other player to make a move\n"

#define ERROR_FIFO "ERROR: FIFO creation failed."
#define ERROR_SIGACTION "ERROR: Sigaction failed."
#define ERROR_FTOK "ERROR: Key creation failed."
#define ERROR_SHM "ERROR: shared memory creation failed."
#define ERROR_SHMAT "ERROR: shared memory attaching failed."
#define ERROR_SHMDT "ERROR: shared memory detaching failed."

/************************************************************************
* Function name: PrintGame                                              *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: prints out the game board.                    *
************************************************************************/
void PrintGame();

/************************************************************************
* Function name: CreateNewGame                                          *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: resets the gameboard to the default starting  *
 * situation and prints it.                                             *
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
* Function name: SwapSymbols                                            *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: swaps the player's and opponent's symbols.    *
************************************************************************/
void SwapSymbols();

/************************************************************************
* Function name: PlaceOpponentTokenOnBoard                              *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols                                                              *
* The output: 1 if the placement is valid, 0 otherwise.                 *
* The Function operation: checks if the token placement is valid in     *
 * in all directions, and if so, makes the placement.                   *
************************************************************************/
int PlaceOpponentTokenOnBoard(int x, int y);

/************************************************************************
* Function name: PlaceTokenOnBoard                                      *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols                                                              *
* The output: 1 if the placement is valid, 0 otherwise.                 *
* The Function operation: checks if the token placement is valid in     *
 * in all directions, and if so, makes the placement.                   *
************************************************************************/
int PlacePlayerTokenOnBoard(int x, int y);

/************************************************************************
* Function name: CountTokenConversions                                  *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols and the direction to count conversions in.                   *
* The output: number of conversions made in this direction.             *
* The Function operation: checks and counts how many token conversions  *
 * are possible in this direction.                                      *
************************************************************************/
int CountTokenConversions(int x, int y, int direction);

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
int IsGameOver();

/************************************************************************
* Function name: IsCoordinatesValid                                     *
* The Input: the input of the player                                    *
* The output: 1 if valid, 0 otherwise                                   *
* The Function operation: checks if the player's input is valid         *
************************************************************************/
int IsCoordinatesValid(char *input);

/************************************************************************
* Function name: Sigusr1Handler                                         *
* The Input: signal number and additional info                          *
* The output: none                                                      *
* The Function operation: connects to the shared memory, determines     *
 * which player the current process is and prints out every move        *
************************************************************************/
void Sigusr1Handler(int sigNum, siginfo_t *info, void *ptr);

/************************************************************************
* Function name: InitializeSigusr1Handler                               *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: Attaches the SIGUSR1 handler to that signal.  *
************************************************************************/
void InitializeSigusr1Handler();

/************************************************************************
* Function name: ExitWithError                                          *
* The Input: an error message                                           *
* The output: none                                                      *
* The Function operation: Prints out the error message and              *
 * exits the program.                                                   *
************************************************************************/
void ExitWithError(char *errorMessage);

char gameBoard[ROWS * COLS];
char playerSymbol, opponentSymbol, latestPlayer;
int fdFifo;

/************************************************************************
* Function name: main                                                   *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: writes the current process's id into the FIFO *
 * and waits for a signal.                                              *
************************************************************************/
int main(int argc, char *argv) {
    // Initialize the signal handler.
    InitializeSigusr1Handler();

    // Create a new board.
    CreateNewGame();

    // Open the FIFO for writing and write the process id.
    if ((fdFifo = open(FIFO_NAME, O_WRONLY)) < 0) {
        ExitWithError(ERROR_FIFO);
    }

    // Write the current process's id into the FIFO.
    pid_t pid = getpid();
    write(fdFifo, &pid, sizeof(pid_t));

    // Wait for a signal to come.
    pause();

    return 0;
}

/************************************************************************
* Function name: PrintGame                                              *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: prints out the game board.                    *
************************************************************************/
void PrintGame() {
    char buffer[1024], tempBuffer[16];
    buffer[0] = '\0';
    int i, j;

    strcat(buffer, "The board is:\n");
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            sprintf(tempBuffer, "%c", gameBoard[COLS * i + j]);
            strcat(buffer, tempBuffer);
        }
        strcat(buffer, "\n");
    }
    strcat(buffer, "\n");

    // Print the game board to stdout.
    write(STDOUT_FILENO, buffer, strlen(buffer));
}

/************************************************************************
* Function name: CreateNewGame                                          *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: resets the gameboard to the default starting  *
 * situation and prints it.                                             *
************************************************************************/
void CreateNewGame() {
    // Reset all the cell to 'O'.
    memset(gameBoard, EMPTY, sizeof(char) * ROWS * COLS);

    // Initialize the game with the default initial black and white cells.
    gameBoard[COLS * 3 + 3] = BLACK;
    gameBoard[COLS * 3 + 4] = WHITE;
    gameBoard[COLS * 4 + 3] = WHITE;
    gameBoard[COLS * 4 + 4] = BLACK;
    PrintGame();
}

/************************************************************************
* Function name: IsGameOver                                             *
* The Input: the player's symbol                                        *
* The output: 1 if the white player won, 2 if the black player won,     *
 * 3 if it's a tie, 0 if the game isn't over                            *
* The Function operation: checks if the game is over, either beacuse    *
 * one of the players won or if it's a tie.                             *
************************************************************************/
int IsGameOver() {
    int x, y;
    char cellSymbol;
    int blackCount = 0, whiteCount = 0, boardIsNotFull = 0,
            blackHasValidPlacement = 0, whiteHasValidPlacement = 0;

    // Iterate through the game board cells
    for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLS; x++) {
            cellSymbol = gameBoard[y * COLS + x];

            // Count the black and white tokens.
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
        ((latestPlayer == WHITE && !blackHasValidPlacement) ||
         (latestPlayer == BLACK && !whiteHasValidPlacement) ||
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
            (input[1] >= 48) && (input[1] <= 56)
            && (input[3] >= 48) && (input[3] <= 56));
}

/************************************************************************
* Function name: Sigusr1Handler                                         *
* The Input: signal number and additional info                          *
* The output: none                                                      *
* The Function operation: connects to the shared memory, determines     *
 * which player the current process is and prints out every move        *
************************************************************************/
void Sigusr1Handler(int sigNum, siginfo_t *info, void *ptr) {
    key_t key;
    int shmid;
    char *pShm, playerLetter, opponentLetter, buffer[8];
    close(fdFifo);

    // Create a new key for shared memory.
    if ((key = ftok("ex31.c", 'k')) == -1) {
        ExitWithError(ERROR_FTOK);
    }

    // Create new shared memory.
    if ((shmid = shmget(key, SHM_SIZE, 0)) == -1) {
        ExitWithError(ERROR_SHM);
    }

    // Attach the process to the shared memory and get a pointer to it.
    pShm = shmat(shmid, NULL, 0);
    if (pShm == (char *) (-1)) {
        ExitWithError(ERROR_SHMAT);
    }

    // Determine if the current process is the black or white player.
    if (strcmp(pShm + 1, "NONE") == 0) {
        playerSymbol = BLACK;
        opponentSymbol = WHITE;
        playerLetter = 'b';
        opponentLetter = 'w';
    } else {
        playerSymbol = WHITE;
        opponentSymbol = BLACK;
        playerLetter = 'w';
        opponentLetter = 'b';

        // Place the other player's token and print the game.
        PlaceOpponentTokenOnBoard((*(pShm + 2) - '0'), (*(pShm + 3) - '0'));
        PrintGame();
    }

    // While the game isn't over, keeping reading the players' moves.
    while (1) {
        // Print a message telling the player to enter coordinates.
        write(STDOUT_FILENO, ENTER_COORDINATES, strlen(ENTER_COORDINATES));

        do {
            int i = 0;
            while ((((*(buffer + (i % 8))) = getchar()) != EOF) &&
                   ((*(buffer + (i % 8))) != '\n')) {
                i++;
            }

            if (!IsCoordinatesValid((char *) buffer)) {
                // If the coordinates are invalid, print out an error message.
                write(STDOUT_FILENO, NO_SUCH_SQUARE, strlen(NO_SUCH_SQUARE));
                continue;
            } else if (!PlacePlayerTokenOnBoard((*(buffer + 1) - '0'),
                                                (*(buffer + 3) - '0'))) {

                // If the coordinates are valid but the placement yields
                // no conversions, print out that it's illegal.
                write(STDOUT_FILENO, INVALID_SQUARE, strlen(INVALID_SQUARE));
                continue;
            } else {
                // Update the shared memory and print the game board.
                *(pShm + 1) = playerLetter;
                *(pShm + 2) = *(buffer + 1);
                *(pShm + 3) = *(buffer + 3);
                *(pShm + 4) = '\0';
                PrintGame();

                // Check if the game is over.
                if (IsGameOver()) {
                    if (shmdt(pShm) == -1) {
                        ExitWithError(ERROR_SHMDT);
                    }
                    return;
                }
                break;
            }
        } while (1);

        // Print a message saying you're waiting for the other player to play.
        write(STDOUT_FILENO, WAITING_FOR_PLAYER, strlen(WAITING_FOR_PLAYER));

        // Wait for the opponent to play.
        while (*(pShm + 1) != opponentLetter) {
            sleep(1);
        }

        // Place the opponent's token on the game board and print the board
        PlaceOpponentTokenOnBoard((*(pShm + 2) - '0'), (*(pShm + 3) - '0'));
        PrintGame();

        // Check if the game is over.
        if (IsGameOver()) {
            if (shmdt(pShm) == -1) {
                ExitWithError(ERROR_SHMDT);
            }
            return;
        }
    }
}

/************************************************************************
* Function name: InitializeSigusr1Handler                               *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: Attaches the SIGUSR1 handler to that signal.  *
************************************************************************/
void InitializeSigusr1Handler() {
    struct sigaction sigAct;

    // Create a block mask for all signal besides
    // SIGUSR1, SIGABRT, SIGINT, SIGTERM
    sigset_t blockMask;
    sigfillset(&blockMask);
    sigdelset(&blockMask, SIGUSR1);
    sigdelset(&blockMask, SIGABRT);
    sigdelset(&blockMask, SIGINT);
    sigdelset(&blockMask, SIGTERM);
    sigAct.sa_mask = blockMask;
    sigAct.sa_flags = SA_SIGINFO;

    // Attach the SIGUSR1 handler to the signal.
    sigAct.sa_sigaction = Sigusr1Handler;
    if (sigaction(SIGUSR1, &sigAct, NULL) != 0) {
        ExitWithError(ERROR_SIGACTION);
    }
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
* Function name: SwapSymbols                                            *
* The Input: none                                                       *
* The output: none                                                      *
* The Function operation: swaps the player's and opponent's symbols.    *
************************************************************************/
void SwapSymbols() {
    char temp = playerSymbol;
    playerSymbol = opponentSymbol;
    opponentSymbol = temp;
}

/************************************************************************
* Function name: PlaceOpponentTokenOnBoard                              *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols                                                              *
* The output: 1 if the placement is valid, 0 otherwise.                 *
* The Function operation: checks if the token placement is valid in     *
 * in all directions, and if so, makes the placement.                   *
************************************************************************/
int PlaceOpponentTokenOnBoard(int y, int x) {
    SwapSymbols();
    PlacePlayerTokenOnBoard(y, x);
    SwapSymbols();
}

/************************************************************************
* Function name: PlacePlayerTokenOnBoard                                *
* The Input: coordinates of token placement, player's and opponent's    *
 * symbols                                                              *
* The output: 1 if the placement is valid, 0 otherwise.                 *
* The Function operation: checks if the token placement is valid in     *
 * in all directions, and if so, makes the placement.                   *
************************************************************************/
int PlacePlayerTokenOnBoard(int x, int y) {
    int index = 0, indexInterval = 0, count, direction, i,
            totalConversions = 0;

    for (direction = UP; direction < DOWN_LEFT + 1; direction++) {
        SetIndexAndInterval(&index, &indexInterval, x, y, direction);
        count = CountTokenConversions(x, y, direction);

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
        latestPlayer = playerSymbol;
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
int CountTokenConversions(int x, int y, int direction) {

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
int IsPlacementValid(int x, int y, char playerSym, char opponentSym) {
    char tempBoard[ROWS * COLS], tempPlayerSym, tempOpponentSym, tempLatest;
    int i, j;

    // Copy the game board into a temporary variable.
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            tempBoard[i * COLS + j] = gameBoard[i * COLS + j];
        }
    }

    tempLatest = latestPlayer;

    // Switch the symbols (if necessary).
    tempPlayerSym = playerSymbol;
    tempOpponentSym = opponentSymbol;
    playerSymbol = playerSym;
    opponentSymbol = opponentSym;

    // Make the placement and get back the number of resulting conversions.
    int result = PlacePlayerTokenOnBoard(x, y);

    // Restore the game board back.
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            gameBoard[i * COLS + j] = tempBoard[i * COLS + j];
        }
    }

    latestPlayer = tempLatest;

    // Switch the symbols back.
    playerSymbol = tempPlayerSym;
    opponentSymbol = tempOpponentSym;

    return result;
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