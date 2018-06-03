//208994962 Shachar Sirotkin
#include <stdio.h>
#include "ex1.h"

//checking whether the computer uses big or little endians by checking
//which byte of a number is the first byte in the memory
int is_little_endian() {
    long number = 1;
    //getting the first byte of the number in the memory
    char* check = (char*)&number;
    //if the byte is the lsb it have to be equal to 1
    if (*check == 1){
        return 1;
    }
    //otherwise the first byte in the memory is not the lsb
    return 0;
}
//replacing the lsb of one number with the other's.
//the function works different relying on big and little endians
unsigned long merge_bytes(unsigned long x, unsigned long int y){
    char YLsb;
    char* XLsb;
    //finding the lsb while using little endian
    if(is_little_endian()){
        YLsb = *(char*)&y;
        XLsb = (char*)&x;
        // finding the lsb while using big endian
    } else {
        YLsb = ((char*)(&y)) [sizeof(unsigned long) - 1];
        XLsb = (char*)(&x+ sizeof(unsigned long) - 1);
    }
    //replacing the lsb
    *XLsb = YLsb;
    return x;
}
// replacing the ith byte in x with b
//the function works different relying on big and little endians
unsigned long put_byte(unsigned long x, unsigned char b, int i){
    if(i>7 || i<0) {
        printf("there is no %d'th byte\n",i);
        return x;
    }
    char *ithByteX;
    //finding the ith byte while using little endian
    if(is_little_endian()) {
        ithByteX = ((char *) &x) + i;
        //finding the ith byte while using big endian
    } else {
        ithByteX = ((char*) &x) + (sizeof(unsigned long) -1 -i);
    }
    //replacing the ith byte with b
    *ithByteX = b;
    return x;
}