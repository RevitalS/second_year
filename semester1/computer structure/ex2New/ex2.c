//208994962 Shachar Sirotkin

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//struct that represents two chars of utf-16 encoding
typedef struct {
    char first;
    char second;
} utf16Char;

//functions' declarations
utf16Char *UtfInit();

void UtfSwap(utf16Char *utf16Char1, utf16Char *utf16Char2);

int isEmptyUtf(utf16Char *utf16Char1);

void copyFile(FILE *src, FILE *dst, int isSwap);

int isLorBEnd(FILE *src, FILE *dst, int isSwap);

void UtfFwrite(utf16Char *utf, FILE *dst, int isSwap);

void changeOs(FILE *src, FILE *dst, char *osSrc, char *osDst, int isSwap);

int validFiles(char **files);

int validFilesAndFlags(char **filesAndFiles);

int validFlags(char **filesAndFlags);

int UtfFread(utf16Char *utf, FILE *src);

int Utfcmp(utf16Char *utf, char *bom);

void unixTo(FILE *src, FILE *dst, char *osDst, int isSwap);

void macTo(FILE *src, FILE *dst, char *osDst, int isSwap);

void winTo(FILE *src, FILE *dst, char *osDst, int isSwap);

void unixToMac(utf16Char *utf, int isLitEnd);

void unixToWin(FILE *pFILE, utf16Char *utf, int isLitEnd, int isSwap);

void Utfcpy(utf16Char *utf, char *cpy);

char *swapBytes(char *line);

void macToUnix(utf16Char *utf, int isLitEnd);

void macToWin(FILE *dst, utf16Char *utf, int isLitEnd, int isSwap);

void winToUnix(FILE *src, FILE *dst, utf16Char *utf, utf16Char *utf2, int isLitEnd, int isSwap);

void winToMac(FILE *src, FILE *dst, utf16Char *utf, utf16Char *utf2, int isLitEnd, int isSwap);

int isSwap(char *swapFlag);

//main function. open the files, check arguments' validation
//and call the 3 options according to them
void main(int argc, char **argv) {
    //open source file and check if it's exist
    FILE *src = fopen(argv[1], "rb");
    if (!src) {
        return;
    }
    FILE *dst = fopen(argv[2], "wb");
    switch (argc) {
        case 3:
            //check the files' validation
            if (validFiles(argv)) {
                //copy without swapping bytes
                //0 mean don't swap the bytes
                copyFile(src, dst, 0);
            }
            break;
        case 5:
            //chack files and flags validation
            if (validFilesAndFlags(argv)) {
                //change os without swapping bytes
                //0 mean don't swap the bytes
                changeOs(src, dst, argv[3], argv[4], 0);
            }
            break;
        case 6:
            //change os and swap bytes according to the swap flag
            changeOs(src, dst, argv[3], argv[4], isSwap(argv[5]));
            break;
        default:
            fclose(src);
            return;
    }
    //close files
    fclose(src);
    fclose(dst);
}

//check the swap flag content
int isSwap(char *swapFlag) {
    if (strcmp(swapFlag, "-swap") == 0) {
        return 1;
    }
    return 0;
}

//check files and flags validation
int validFilesAndFlags(char **filesAndFiles) {
    if (validFiles(filesAndFiles) && validFlags(filesAndFiles)) {
        return 1;
    }
    return 0;
}

//check the os flags validation
int validFlags(char **filesAndFlags) {
    //check whether the os flags are valid
    if (strcmp(filesAndFlags[3], "-win") || strcmp(filesAndFlags[3], "-unix")
        || strcmp(filesAndFlags[3], "-mac")) {
        if (strcmp(filesAndFlags[4], "-win") || strcmp(filesAndFlags[4], "-unix")
            || strcmp(filesAndFlags[4], "-mac")) {
            return 1;
        }
    }
    return 0;
}
//check files validation
int validFiles(char **files) {
    return strstr(files[1], ".") && strstr(files[2], ".") ? 1 : 0;
}

//creates file with fit endlines according to the os and swap flags
void changeOs(FILE *src, FILE *dst, char *osSrc, char *osDst, int isSwap) {
    if (strcmp(osSrc, "-unix") == 0) {
        //same os meant normal copying
        if (strcmp(osDst, "-unix") == 0) {
            copyFile(src, dst, isSwap);
        } else {
            unixTo(src, dst, osDst, isSwap);
        }
    } else if (strcmp(osSrc, "-mac") == 0) {
        //same os meant normal copying
        if (strcmp(osDst, "-mac") == 0) {
            copyFile(src, dst, isSwap);
        } else {
            macTo(src, dst, osDst, isSwap);
        }
    } else if (strcmp(osSrc, "-win") == 0) {
        //same os meant normal copying
        if (strcmp(osDst, "-win") == 0) {
            copyFile(src, dst, isSwap);
        } else {
            winTo(src, dst, osDst, isSwap);
        }
    }
}

//change os from windows to mac and unix according to the os destination
void winTo(FILE *src, FILE *dst, char *osDst, int isSwap) {
    utf16Char *utf = UtfInit();
    utf16Char *utf2 = UtfInit();
    //check for little or big endians
    int isLitEnd = isLorBEnd(src, dst, isSwap);
    while (UtfFread(utf, src)) {
        //change to unix
        if (strcmp(osDst, "-unix") == 0) {
            winToUnix(src, dst, utf, utf2, isLitEnd, isSwap);
        } else {
            //change to mac
            winToMac(src, dst, utf, utf2, isLitEnd, isSwap);
        }
        UtfFwrite(utf, dst, isSwap);
    }
    free(utf);
    free(utf2);
}

//change os from windows to mac
void winToMac(FILE *src, FILE *dst, utf16Char *utf, utf16Char *utf2, int isLitEnd, int isSwap) {
    char endLine[2];
    endLine[0] = '\0';
    endLine[1] = '\r';
    char endLine2[2];
    endLine2[0] = '\0';
    endLine2[1] = '\n';
    //for little endiens
    if (isLitEnd) {
        if (Utfcmp(utf, endLine) == 0) {
            if (UtfFread(utf2, src) != 0) {
                //change the endlines to mac's enline
                if (Utfcmp(utf2, endLine2) == 0) {
                    Utfcpy(utf, endLine);
                } else {
                    //if the second utf16Char iss not \n
                    UtfSwap(utf, utf2);
                    UtfFwrite(utf2, dst, isSwap);
                }
            }
        }
    //for big endiens
    } else {
        if (Utfcmp(utf, swapBytes(endLine)) == 0) {
            if (UtfFread(utf2, src) != 0) {
                if (Utfcmp(utf2, swapBytes(endLine2)) == 0) {
                    Utfcpy(utf, endLine);
                } else {
                    //if the second utf16Char iss not \n
                    UtfSwap(utf, utf2);
                    UtfFwrite(utf2, dst, isSwap);
                }
            }
        }
    }
}

//change os from windows to unix
void winToUnix(FILE *src, FILE *dst, utf16Char *utf, utf16Char *utf2, int isLitEnd, int isSwap) {
    char endLine[2];
    endLine[0] = '\0';
    endLine[1] = '\r';
    char endLine2[2];
    endLine2[0] = '\0';
    endLine2[1] = '\n';
    //for little endiens
    if (isLitEnd) {
        if (Utfcmp(utf, endLine) == 0) {
            if (UtfFread(utf2, src) != 0) {
                if (Utfcmp(utf2, endLine2) == 0) {
                    Utfcpy(utf, endLine2);
                } else {
                    UtfSwap(utf, utf2);
                    UtfFwrite(utf2, dst, isSwap);
                }
            }
        }
        //for big endiens
    } else {
        if (Utfcmp(utf, swapBytes(endLine)) == 0) {
            if (UtfFread(utf2, src) != 0) {
                if (Utfcmp(utf2, swapBytes(endLine2)) == 0) {
                    Utfcpy(utf, endLine2);
                } else {
                    UtfSwap(utf, utf2);
                    UtfFwrite(utf2, dst, isSwap);
                }
            }
        }
    }
}

//change os from mac to windows and unix according to the os destination
void macTo(FILE *src, FILE *dst, char *osDst, int isSwap) {
    int isLitEnd = isLorBEnd(src, dst, isSwap);
    utf16Char *utf = UtfInit();
    while (UtfFread(utf, src)) {
        if (strcmp(osDst, "-unix") == 0) {
            macToUnix(utf, isLitEnd);
        } else {
            macToWin(dst, utf, isLitEnd, isSwap);
        }
        UtfFwrite(utf, dst, isSwap);
    }
    free(utf);
}

//change os from mac to windows
void macToWin(FILE *dst, utf16Char *utf, int isLitEnd, int isSwap) {
    char endLine[2];
    endLine[0] = '\0';
    endLine[1] = '\r';
    char endLine2[2];
    endLine2[0] = '\0';
    endLine2[1] = '\n';
    utf16Char *utf2 = UtfInit();
    if (isLitEnd) {
        //for little endiens
        if (Utfcmp(utf, endLine) == 0) {
            Utfcpy(utf2, endLine2);
        }
    } else {
        //for big endiens
        if (Utfcmp(utf, swapBytes(endLine)) == 0) {
            Utfcpy(utf2, swapBytes(endLine2));
        }
    }
    //check whether second utfChar was read
    if (!isEmptyUtf(utf2)) {
        UtfSwap(utf, utf2);
        UtfFwrite(utf2, dst, isSwap);
    }
    free(utf2);
}

//change os from mac to unix
void macToUnix(utf16Char *utf, int isLitEnd) {
    char endLine[2];
    endLine[0] = '\0';
    endLine[1] = '\r';
    char endLine2[2];
    endLine2[0] = '\0';
    endLine2[1] = '\n';
    if (isLitEnd) {
        //for little endiens
        if (Utfcmp(utf, endLine) == 0) {
            Utfcpy(utf, endLine2);
        }
    } else {
        //for big endiens
        if (Utfcmp(utf, swapBytes(endLine)) == 0) {
            Utfcpy(utf, swapBytes(endLine2));
        }
    }
}

//change os from unix to windows and mac according to the os destination
void unixTo(FILE *src, FILE *dst, char *osDst, int isSwap) {
    int isLitEnd = isLorBEnd(src, dst, isSwap);
    utf16Char *utf = UtfInit();
    while (UtfFread(utf, src)) {
        if (strcmp(osDst, "-mac") == 0) {
            unixToMac(utf, isLitEnd);
        } else {
            unixToWin(dst, utf, isLitEnd, isSwap);
        }
        UtfFwrite(utf, dst, isSwap);
    }
    free(utf);
}

//change os from unix to windows
void unixToWin(FILE *dst, utf16Char *utf, int isLitEnd, int isSwap) {
    char endLine2[2];
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char endLine[2];
    endLine[0] = '\0';
    endLine[1] = '\n';
    utf16Char *utf2 = UtfInit();
    if (isLitEnd) {
        //for little endiens
        if (Utfcmp(utf, endLine) == 0) {
            Utfcpy(utf, endLine2);
            Utfcpy(utf2, endLine);
        }
    } else {
        //for big endiens
        if (Utfcmp(utf, swapBytes(endLine)) == 0) {
            Utfcpy(utf, swapBytes(endLine2));
            Utfcpy(utf2, endLine);
        }
    }
    //check whether second utfChar was read
    if (!isEmptyUtf(utf2)) {
        UtfSwap(utf, utf2);
        UtfFwrite(utf2, dst, isSwap);
    }
    free(utf2);
}

//change os from unix to mac
void unixToMac(utf16Char *utf, int isLitEnd) {
    char endLine2[2];
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char endLine[2];
    endLine[0] = '\0';
    endLine[1] = '\n';
    if (isLitEnd) {
        if (Utfcmp(utf, endLine) == 0) {
            Utfcpy(utf, endLine2);
        }
    } else {
        if (Utfcmp(utf, swapBytes(endLine)) == 0) {
            Utfcpy(utf, swapBytes(endLine2));
        }
    }
}

//swap the bytes of the utfChar
char *swapBytes(char *line) {
    char temp = line[0];
    *line = *(line + 1);
    line[1] = temp;
    return line;
}

//creates copy in the same os
//swap bytes according to the swap flag
void copyFile(FILE *src, FILE *dst, int isSwap) {
    utf16Char *utf = UtfInit();
    while (UtfFread(utf, src)) {
        UtfFwrite(utf, dst, isSwap);
    }
    free(utf);
}

//read utf16 char
int UtfFread(utf16Char *utf, FILE *src) {
    int count = 0;
    if (fread(&utf->first, 1, 1, src)) {
        count++;
    }
    if (fread(&utf->second, 1, 1, src)) {
        count++;
    }
    return count;
}

//write utf16 char
void UtfFwrite(utf16Char *utf, FILE *dst, int isSwap) {
    if (isSwap) {
        fwrite(&utf->second, 1, 1, dst);
        fwrite(&utf->first, 1, 1, dst);
    } else {
        fwrite(&utf->first, 1, 1, dst);
        fwrite(&utf->second, 1, 1, dst);
    }
}

//compare utf16 char with two bytes
int Utfcmp(utf16Char *utf, char string[2]) {
    if (utf->first == string[1] && utf->second == string[0]) {
        return 0;
    }
    return 1;
}

//copy two bytes into utf16 char
void Utfcpy(utf16Char *utf, char cpy[2]) {
    utf->first = cpy[1];
    utf->second = cpy[0];
}

//check for little or big endiens
int isLorBEnd(FILE *src, FILE *dst, int isSwap) {
    utf16Char *utf = UtfInit();
    //check the file's BOM
    char *BOM = "\xff\xfe";
    if (UtfFread(utf, src)) {
        if (Utfcmp(utf, BOM) == 0) {
            UtfFwrite(utf, dst, isSwap);
            free(utf);
            //big endian
            return 0;
        }
        UtfFwrite(utf, dst, isSwap);
        free(utf);
        //little endian
        return 1;
    }
    free(utf);
    return 0;
}

//allocates memory for a utf16 char and create an empty one
utf16Char *UtfInit() {
    utf16Char *utf = (utf16Char *) malloc(sizeof(utf16Char));
    utf->first = '\0';
    utf->second = '\0';
    return utf;
}

//check whether utf16 char is empty
int isEmptyUtf(utf16Char *utf16Char1) {
    if (utf16Char1->first == '\0' && utf16Char1->second == '\0') {
        return 1;
    }
    return 0;
}

//swap between two utf16 chars
void UtfSwap(utf16Char *utf16Char1, utf16Char *utf16Char2) {
    utf16Char temp = *utf16Char1;
    utf16Char1->first = utf16Char2->first;
    utf16Char1->second = utf16Char2->second;
    utf16Char2->first = temp.first;
    utf16Char2->second = temp.second;
}