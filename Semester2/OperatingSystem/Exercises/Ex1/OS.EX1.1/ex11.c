/*********************************
 * Student Name: Shachar sirotkin
 * Student ID: 208994962
 * Course Exercise Group: 06
 * Excecise name: ex12.c
 *********************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 512

/***
 * func name: ReadFileToBuffer
 * input: fd - file descriptor to a file needed to be read to a buffer
 *        buffer - the buffer thath the file is loaded into.
 * output: 1 if there was no problem, -1 if there was one.
 * func operation: read the whole file into the buffer
 */
int ReadFileToBuffer(int fd, char **buffer);

/***
 * func name: Finish
 * input: fd1 - the first file descritor
 *        fd2 - the second file descritor
 *        buffer1 - a buffer
 *        buffer2 - another buffer
 * output: no out put
 * func operation: free the buffers and close the file descritors
 */
void Finish(int fd1, int fd2, char **buffer1, char **buffer2);

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char *s);

/***
 * func name: main
 * input: argc - the number of the args
 *        argv - the args
 * return 0
 * func operation: open the both files, read them and check whether the files
 * identical, similar or different
 */
int main(int argc, char *argv[]) {
    //finishes the process with wrong input
    if (argc != 3) {
        exit(-1);
    }

    //opens the files
    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);
    //if the open system call failed, write it to the stderr
    CheckError(fd1, "open file one failed");
    CheckError(fd2, "open file two failed");
    //allocates buffer for reading the files
    char *buffer1 = (char *) malloc(BUFFER_SIZE * sizeof(char));
    char *buffer2 = (char *) malloc(BUFFER_SIZE * sizeof(char));
    //read the first file to buffer
    if (ReadFileToBuffer(fd1, &buffer1) == -1) {
        perror("read file one failed");
        //free buffers and close file descriptors
        Finish(fd1, fd2, &buffer1, &buffer2);
        exit(-1);
    }
    //read the second file to buffer
    if (ReadFileToBuffer(fd2, &buffer2) == -1) {
        perror("read file two failed");
        //free buffers and close file descriptors
        Finish(fd1, fd2, &buffer1, &buffer2);
        exit(-1);
    }
    //return 1 if the file are equal
    if (strcmp(buffer1, buffer2) == 0) {
        //free buffers and close file descriptors
        Finish(fd1, fd2, &buffer1, &buffer2);
        printf("should return 1");
        return 1;
    }
    char *tempBuffer1 = buffer1;
    char *tempBuffer2 = buffer2;
    //running over the file and checking whether they are similar or not
    while (*tempBuffer1 != '\0' && *tempBuffer2 != '\0') {
        //skip on every space and new line char
        while (*tempBuffer1 == ' ' || *tempBuffer1 == '\n' ||
               *tempBuffer1 == '\t') {
            tempBuffer1++;
        }
        while (*tempBuffer2 == ' ' || *tempBuffer2 == '\n' ||
               *tempBuffer2 == '\t') {
            tempBuffer2++;
        }
        //check whether the chars after the skipping are equal
        if (*tempBuffer1 == *tempBuffer2) {
            tempBuffer1++;
            tempBuffer2++;
            continue;
        }
        //check whether the chars are matching upper and lower case
        if (isalpha(*tempBuffer1) && isalpha(*tempBuffer2)) {
            if (toupper(*tempBuffer1) == toupper(*tempBuffer2)) {
                tempBuffer1++;
                tempBuffer2++;
                continue;
            }
        }
        //free buffers and close file descriptors
        Finish(fd1, fd2, &buffer1, &buffer2);
        printf("should return 3");
        return 3;
    }
    //if only one of the files ends,
    // the file don't contain the same amount of chars
    if ((*buffer1 == '\0' && *buffer2 != '\0') ||
        (*buffer1 != '\0' && *buffer2 == '\0')) {
        //free buffers and close file descriptors
        Finish(fd1, fd2, &buffer1, &buffer2);
        printf("should return 3");
        return 3;
    }
    //free buffers and close file descriptors
    Finish(fd1, fd2, &buffer1, &buffer2);
    printf("should return 2");
    return 2;
}

/***
 * func name: ReadFileToBuffer
 * input: fd - file descriptor to a file needed to be read to a buffer
 *        buffer - the buffer thath the file is loaded into.
 * output: 1 if there was no problem, -1 if there was one.
 * func operation: read the whole file into the buffer
 */
int ReadFileToBuffer(int fd, char **buffer) {
    //for indicating the new size for reallocation
    // and for reading the next chars to the end of the buffer
    int readingTimes = 0;
    ssize_t readResult;
    do {
        //read BUFFER_SIZE chars from the file to the
        readResult = read(fd, (*buffer + readingTimes * BUFFER_SIZE),
                          BUFFER_SIZE);
        CheckError((int) readResult, "reading from file failed");
        //if chars were read to the buffer, reallocate it
        if (readResult > 0) {
            ++readingTimes;
            *buffer = realloc(*buffer,
                              (size_t) (readingTimes + 1) * BUFFER_SIZE);
            if (*buffer == NULL) {
                return -1;
            }
        }
    } while (readResult != 0);
    return 1;
}

/***
 * func name: Finish
 * input: fd1 - the first file descritor
 *        fd2 - the second file descritor
 *        buffer1 - a buffer
 *        buffer2 - another buffer
 * output: no out put
 * func operation: free the buffers and close the file descritors
 */
void Finish(int fd1, int fd2, char **buffer1, char **buffer2) {
    //free buffers and close file descriptors
    free(*buffer1);
    free(*buffer2);
    close(fd1);
    close(fd2);
}

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char *s) {
    //if x is -1, writing s to stderr
    if (x == -1) {
        size_t i = strlen(s);
        write(2, s, i);
        exit(-1);
    }
}