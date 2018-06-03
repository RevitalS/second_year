#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

void swap(char **buf, char **buf2);

void copyFile(FILE *fr, FILE *fw, int isSwap);

int isLorBEnd(FILE *src, FILE *dst, int isSwap);

char *swapBytes(char *bytes);

void unixTo(FILE *src, FILE *dst, char *osDst, int isSwap);

void unixToMac(char *buf, int isLitEnd);

void unixToWin(FILE *dst, char *buf, int isLitEnd, int isSwap);

void macTo(FILE *src, FILE *dst, char *osDst, int isSwap);

void macToUnix(char *buf, int isLitEnd);

void macToWin(FILE *dst, char *buf, int isLitEnd, int isSwap);

void winTo(FILE *src, FILE *dst, char *osDst, int isSwap);

void winToUnix(FILE *src, FILE *dst, char *buf, char *buf2, int isLitEnd, int isSwap);

void winToMac(FILE *src, FILE *dst, char *buf, char *buf2, int isLitEnd, int isSwap);

void changeOsFile(FILE *src, FILE *dst, char *osSrc, char *OsDst, int isSwap);

int fitToM1(char **string);

int fitToM2(char **pString);

int checkOsFlag(char *string);

int checkSwap(char *order);

void copyFile(FILE *fr, FILE *fw, int isSwap) {
    char *buf = (char *) malloc(1 * sizeof(char));
    while (fread(buf, 1, 1, fr) != 0) {
        if (isSwap) {
            swapBytes(buf);
        }
        fwrite(buf, 1, 1, fw);
    }
    free(buf);
}

int isLorBEnd(FILE *src, FILE *dst, int isSwap) {
    char *buf = (char *) malloc(2 * sizeof(char));
    char *BOM = "\xff\xfe";
    if (fread(buf, 2, 1, src) != 0) {
        if (strcmp(buf, BOM) == 0) {
            if (isSwap) {
                swapBytes(buf);
            }
            fwrite((void *) buf, 2, 1, dst);
            free(buf);
            //little endian
            return 1;
        }
        if (isSwap) {
            swapBytes(buf);
        }
        fwrite(buf, 2, 1, dst);
        free(buf);
        //big endian
        return 0;
    }
    free(buf);
    return 0;
}

void unixToMac(char *buf, int isLitEnd) {
    char *endLine2 = (char *) malloc(2 * sizeof(char));
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char *endLine = (char *) malloc(2 * sizeof(char));
    endLine[0] = '\0';
    endLine[1] = '\n';
    if (isLitEnd) {
        if (strcmp(buf, "\n") == 0) {
            strcpy(buf, "\r");
        }
    } else {
        if (strcmp(buf, swapBytes(endLine)) == 0) {
            strcpy(buf, swapBytes(endLine2));
        }
    }
    free(endLine);
    free(endLine2);
}

void unixToWin(FILE *dst, char *buf, int isLitEnd, int isSwap) {
    char *endLine2 = (char *) malloc(2 * sizeof(char));
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char *endLine = (char *) malloc(2 * sizeof(char));
    endLine[0] = '\0';
    endLine[1] = '\n';
    char *buf2 = (char *) malloc(2 * sizeof(char));
    if (isLitEnd) {
        if (strcmp(buf, "\n") == 0) {
            strcpy(buf, "\r");
            strcpy(buf2, "\n");
        }
    } else {
        if (strcmp(buf, swapBytes(endLine)) == 0) {
            strcpy(buf, swapBytes(endLine2));
            strcpy(buf2, swapBytes(endLine));
        }
    }
    if (strlen(buf2) != 0) {
        swap(&buf, &buf2);
        if (isSwap) {
            swapBytes(buf2);
        }
        fwrite(buf2, 2, 1, dst);
    }
    free(buf2);
    free(endLine);
    free(endLine2);
}

void macToWin(FILE *dst, char *buf, int isLitEnd, int isSwap) {
    char *endLine2 = (char *) malloc(2 * sizeof(char));
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char *endLine = (char *) malloc(2 * sizeof(char));
    endLine[0] = '\0';
    endLine[1] = '\n';
    char *buf2 = (char *) malloc(2 * sizeof(char));
    if (isLitEnd) {
        if (strcmp(buf, "\r") == 0) {
            strcpy(buf2, "\n");
        }
    } else {
        if (strcmp(buf, swapBytes(endLine2)) == 0) {
            strcpy(buf2, swapBytes(endLine));
        }
    }
    if (strlen(buf2) != 0) {
        swap(&buf, &buf2);
        if (isSwap) {
            swapBytes(buf2);
        }
        fwrite(buf2, 2, 1, dst);
    }
    free(buf2);
    free(endLine);
    free(endLine2);
}

void macToUnix(char *buf, int isLitEnd) {
    char *endLine = (char *) malloc(2* sizeof(char));
    endLine[0] = '\0';
    endLine[1] = '\r';
    char *endLine2 = (char *) malloc(2 * sizeof(char));
    endLine[0] = '\0';
    endLine2[1] = '\n';
    if (isLitEnd) {
        if (strcmp(buf, "\r") == 0) {
            strcpy(buf, "\n");
        }
    } else if (strcmp(buf, swapBytes(endLine)) == 0) {
        strcpy(buf, swapBytes(endLine2));
    }
    free(endLine);
    free(endLine2);
}

void swap(char **buf, char **buf2) {
    char *temp = *buf2;
    *buf2 = *buf;
    *buf = temp;
}

void winToMac(FILE *src, FILE *dst, char *buf, char *buf2, int isLitEnd, int isSwap) {
    char *endLine2 = (char *) malloc(2 * sizeof(char));
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char *endLine = (char *) malloc(2 * sizeof(char));
    endLine[0] = '\0';
    endLine[1] = '\n';
    if (isLitEnd) {
        if (strcmp(buf, "\r") == 0) {
            if (fread(buf, 2, 1, src) != 0) {
                if (strcmp(buf2, "\n")) {
                    swap(&buf, &buf2);
                    if (isSwap) {
                        swapBytes(buf2);
                    }
                    fwrite((void *) buf2, 2, 1, dst);
                }
            }
        }
    } else {
        if (strcmp(buf, swapBytes(endLine2)) == 0) {
            if (fread(buf, 2, 1, src) != 0) {
                if (strcmp(buf2, swapBytes(endLine))) {
                    swap(&buf, &buf2);
                    if (isSwap) {
                        swapBytes(buf2);
                    }
                    fwrite((void *) buf2, 2, 1, dst);
                }
            }
        }
    }
    free(endLine);
    free(endLine2);
}

void winToUnix(FILE *src, FILE *dst, char *buf, char *buf2, int isLitEnd, int isSwap) {
    char *endLine2 = (char *) malloc(2 * sizeof(char));
    endLine2[0] = '\0';
    endLine2[1] = '\r';
    char *endLine = (char *) malloc(2 * sizeof(char));
    endLine[0] = '\0';
    endLine[1] = '\n';
    if (isLitEnd) {
        if (strcmp(buf, "\r") == 0) {
            if (fread(buf2, 2, 1, src) != 0) {
                if (strcmp(buf2, "\n") == 0) {
                    strcpy(buf, "\n");
                } else {
                    swap(&buf, &buf2);
                    if (isSwap) {
                        swapBytes(buf2);
                    }
                    fwrite((void *) buf2, 2, 1, dst);
                }
            }
        }
    } else {
        if (strcmp(buf, swapBytes(endLine2)) == 0) {
            if (fread(buf2, 2, 1, src) != 0) {
                if (strcmp(buf2, swapBytes(endLine)) == 0) {
                    strcpy(buf, swapBytes(endLine));
                } else {
                    swap(&buf, &buf2);
                    if (isSwap) {
                        swapBytes(buf2);
                    }
                    fwrite((void *) buf2, 2, 1, dst);
                }
            }
        }
    }
}

void changeOsFile(FILE *src, FILE *dst, char *osSrc, char *osDst, int isSwap) {
    if (strcmp(osSrc, "-unix") == 0) {
        if (strcmp(osDst, "-unix") == 0) {
            copyFile(src, dst, isSwap);
        } else {
            unixTo(src, dst, osDst, isSwap);
        }
    } else if (strcmp(osSrc, "-mac") == 0) {
        if (strcmp(osDst, "-mac") == 0) {
            copyFile(src, dst, isSwap);
        } else {
            macTo(src, dst, osDst, isSwap);
        }
    } else if (strcmp(osSrc, "-win") == 0) {
        if (strcmp(osDst, "-win") == 0) {
            copyFile(src, dst, isSwap);
        } else {
            winTo(src, dst, osDst, isSwap);
        }
    }
}

char *swapBytes(char *bytes) {
    char temp = bytes[0];
    *bytes = *(bytes + 1);
    bytes[1] = temp;
    return bytes;
}

void unixTo(FILE *src, FILE *dst, char *osDst, int isSwap) {
    char *buf = (char *) malloc(2 * sizeof(char));
    int isLitEnd = isLorBEnd(src, dst, isSwap);
    while (fread(buf, 2, 1, src) != 0) {
        if (strcmp(osDst, "-mac") == 0) {
            unixToMac(buf, isLitEnd);
        } else {
            unixToWin(dst, buf, isLitEnd, isSwap);
        }
        if (isSwap) {
            swapBytes(buf);
        }
        fwrite(buf, 2, 1, dst);
    }
    free(buf);
}

int checkSwap(char *order) {
    if (strcmp(order, "-swap")) {
        return 1;
    }
    return 0;
}

void macTo(FILE *src, FILE *dst, char *osDst, int isSwap) {
    char *buf = (char *) malloc(2 * sizeof(char));
    int isLitEnd = isLorBEnd(src, dst, isSwap);
    while (fread(buf, 2, 1, src) != 0) {
        if (strcmp(osDst, "-unix") == 0) {
            macToUnix(buf, isLitEnd);
        } else {
            macToWin(dst, buf, isLitEnd, isSwap);
        }
        if (isSwap) {
            swapBytes(buf);
        }
        fwrite(buf, 2, 1, dst);
    }
    free(buf);
}

void winTo(FILE *src, FILE *dst, char *osDst, int isSwap) {
    char *buf = (char *) malloc(2 * sizeof(char));
    char *buf2 = (char *) malloc(2 * sizeof(char));
    int isLitEnd = isLorBEnd(src, dst, isSwap);
    while (fread(buf, 2, 1, src) != 0) {
        if (strcmp(osDst, "-unix") == 0) {
            winToUnix(src, dst, buf, buf2, isLitEnd, isSwap);
        } else {
            winToMac(src, dst, buf, buf2, isLitEnd, isSwap);
        }
        if (isSwap) {
            swapBytes(buf);
        }
        fwrite(buf, 2, 1, dst);
    }
    free(buf);
    free(buf2);
}


void main(int argc, char **argv) {
    if (argc <= 1 || argc == 4) {
        return;
    }
    FILE *fr;
    FILE *fw;
    switch (argc) {
        case 3:
            if (fitToM1(argv)) {
                fr = fopen(argv[1], "rb");
                fw = fopen(argv[2], "wb");
                if (!fw || !fr) {
                    return;
                }
                copyFile(fr, fw, 0);
                break;
            } else {
                return;
            }
        case 5:
            if (fitToM2(argv)) {
                fr = fopen(argv[1], "rb");
                fw = fopen(argv[2], "wb");
                if (!fw || !fr) {
                    return;
                }
                changeOsFile(fr, fw, argv[3], argv[4], 0);
                break;
            } else {
                return;
            }
        case 6:
            fr = fopen(argv[1], "rb");
            fw = fopen(argv[2], "wb");
            if (!fw || !fr) {
                return;
            }
            changeOsFile(fr, fw, argv[3], argv[4], checkSwap(argv[5]));
            break;
        default:
            return;
    }
    fclose(fw);
    fclose(fr);
}

int fitToM2(char **pString) {
    if (fitToM1(pString) && checkOsFlag(pString[3]) && checkOsFlag(pString[4])) {
        return 1;
    }
    return 0;
}

int checkOsFlag(char *string) {
    if (strcmp(string, "-unix") || strcmp(string, "-mac") || strcmp(string, "-win")) {
        return 1;
    }
    return 0;
}

int fitToM1(char **fileNames) {
    if (strstr(fileNames[1], ".") && strstr(fileNames[2], ".")) {
        return 1;
    }
    return 0;
}