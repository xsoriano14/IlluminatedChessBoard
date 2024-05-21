#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include "LED_Matrix.h"
#include <math.h>

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x70 //LED Matrix Device 

#define NUMOFDIGITS 10

//struct to hold info for each digit
struct DigitInfo {
    int digit;
    unsigned char digit_bytes[7]; // holds hardcoded byte values 
};


static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

//initialize I2C Bus
static int initI2cBus(char* bus, int address)
{
    //configure pins to i2c mode
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");

    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);    
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

//write to I2C device
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

//resets the display to all zeros
void reset_Display(){
    unsigned char init[] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000, 0b00000000};
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    int j = 0;
    for (int i = 0x00; i <= 0x0f; i+=2) {
        writeI2cReg(i2cFileDesc, i, init[j++]);
    }
}

void turnOnDisplay(){
    runCommand("i2cset -y 1 0x70 0x21 0x00");
    runCommand("i2cset -y 1 0x70 0x81 0x00");
}

void turnOffDisplay(){
    runCommand("i2cset -y 1 0x70 0x80 0x00");
}

//returns the appropriate an array of bytes to display a particular digit on the display
static unsigned char* getBytesArray(int number){
    struct DigitInfo digits[10]; //array of structs
    
    // struct DigitInfo digits = {
    // {0, {0b11100000, 0b10100000, 0b10100000,0b10100000,0b10100000,0b10100000,0b11100000}},
    // {1, {0b01000000,0b11000000,0b01000000,0b01000000,0b01000000,0b01000000,0b11100000}},
    // {2, {0b11100000,0b00100000,0b00100000,0b11100000,0b10000000,0b10000000,0b11100000}},
    // {3, {0b11100000,0b00100000,0b00100000,0b11100000,0b00100000,0b00100000,0b11100000}},
    // {4, {0b10100000,0b10100000,0b10100000,0b11100000,0b00100000,0b00100000,0b00100000}},
    // {5, {0b11100000,0b10000000,0b10000000,0b11100000,0b00100000,0b00100000,0b11100000}},
    // {6, {0b11100000,0b10000000,0b10000000,0b11100000,0b10100000,0b10100000,0b11100000}},
    // {7, {0b11110000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000}},
    // {8, {0b11100000,0b10100000,0b10100000,0b11100000,0b10100000,0b10100000,0b11100000}},
    // {9, {0b11100000,0b10100000,0b10100000,0b11100000,0b00100000,0b00100000,0b00100000}},
    // };

    //HARDCODED VALUES FOR EACH DIGIT  (0 - 9)

    digits[0].digit = 0; 
    digits[0].digit_bytes[0] = 0b11100000;
    digits[0].digit_bytes[1] = 0b10100000;
    digits[0].digit_bytes[2] = 0b10100000;
    digits[0].digit_bytes[3] = 0b10100000;
    digits[0].digit_bytes[4] = 0b10100000;
    digits[0].digit_bytes[5] = 0b10100000;
    digits[0].digit_bytes[6] = 0b11100000;


    digits[1].digit = 1;
    digits[1].digit_bytes[0] = 0b01000000;
    digits[1].digit_bytes[1] = 0b11000000;
    digits[1].digit_bytes[2] = 0b01000000;
    digits[1].digit_bytes[3] = 0b01000000;
    digits[1].digit_bytes[4] = 0b01000000;
    digits[1].digit_bytes[5] = 0b01000000;
    digits[1].digit_bytes[6] = 0b11100000;

    digits[2].digit = 2;
    digits[2].digit_bytes[0] = 0b11100000;
    digits[2].digit_bytes[1] = 0b00100000;
    digits[2].digit_bytes[2] = 0b00100000;
    digits[2].digit_bytes[3] = 0b11100000;
    digits[2].digit_bytes[4] = 0b10000000;
    digits[2].digit_bytes[5] = 0b10000000;
    digits[2].digit_bytes[6] = 0b11100000;

    digits[3].digit = 3;
    digits[3].digit_bytes[0] = 0b11100000;
    digits[3].digit_bytes[1] = 0b00100000;
    digits[3].digit_bytes[2] = 0b00100000;
    digits[3].digit_bytes[3] = 0b11100000;
    digits[3].digit_bytes[4] = 0b00100000;
    digits[3].digit_bytes[5] = 0b00100000;
    digits[3].digit_bytes[6] = 0b11100000;

    digits[4].digit = 4;
    digits[4].digit_bytes[0] = 0b10100000;
    digits[4].digit_bytes[1] = 0b10100000;
    digits[4].digit_bytes[2] = 0b10100000;
    digits[4].digit_bytes[3] = 0b11100000;
    digits[4].digit_bytes[4] = 0b00100000;
    digits[4].digit_bytes[5] = 0b00100000;
    digits[4].digit_bytes[6] = 0b00100000;

    digits[5].digit = 5;
    digits[5].digit_bytes[0] = 0b11100000;
    digits[5].digit_bytes[1] = 0b10000000;
    digits[5].digit_bytes[2] = 0b10000000;
    digits[5].digit_bytes[3] = 0b11100000;
    digits[5].digit_bytes[4] = 0b00100000;
    digits[5].digit_bytes[5] = 0b00100000;
    digits[5].digit_bytes[6] = 0b11100000;


    digits[6].digit = 6;
    digits[6].digit_bytes[0] = 0b11100000;
    digits[6].digit_bytes[1] = 0b10000000;
    digits[6].digit_bytes[2] = 0b10000000;
    digits[6].digit_bytes[3] = 0b11100000;
    digits[6].digit_bytes[4] = 0b10100000;
    digits[6].digit_bytes[5] = 0b10100000;
    digits[6].digit_bytes[6] = 0b11100000;

    digits[7].digit = 7;
    digits[7].digit_bytes[0] = 0b11100000;
    digits[7].digit_bytes[1] = 0b00100000;
    digits[7].digit_bytes[2] = 0b00100000;
    digits[7].digit_bytes[3] = 0b00100000;
    digits[7].digit_bytes[4] = 0b00100000;
    digits[7].digit_bytes[5] = 0b00100000;
    digits[7].digit_bytes[6] = 0b00100000;

    digits[8].digit = 8;
    digits[8].digit_bytes[0] = 0b11100000;
    digits[8].digit_bytes[1] = 0b10100000;
    digits[8].digit_bytes[2] = 0b10100000;
    digits[8].digit_bytes[3] = 0b11100000;
    digits[8].digit_bytes[4] = 0b10100000;
    digits[8].digit_bytes[5] = 0b10100000;
    digits[8].digit_bytes[6] = 0b11100000;

    digits[9].digit = 9;
    digits[9].digit_bytes[0] = 0b11100000;
    digits[9].digit_bytes[1] = 0b10100000;
    digits[9].digit_bytes[2] = 0b10100000;
    digits[9].digit_bytes[3] = 0b11100000;
    digits[9].digit_bytes[4] = 0b00100000;
    digits[9].digit_bytes[5] = 0b00100000;
    digits[9].digit_bytes[6] = 0b00100000;

    unsigned char* result = malloc(7 * sizeof(*result)); //dynamic allocation to return array

    if(result != NULL) {
        // Copy the content of digit_bytes to the dynamically allocated memory
        for (int i = 0; i < 7; ++i) {
            result[i] = digits[number].digit_bytes[i];
        }
    }

    return result;

}

// //rotates the given bits to the right by shift value
// static unsigned char rotateLeft(unsigned char value, int shift) {
//     return (value << shift) | (value >> (8 - shift));
// }

static unsigned char rotateRight(unsigned char value, int shift) {
    return (value >> shift) | (value << (8 - shift));
}
//Maps the given byte to the LED display
static unsigned char mapBitsToLeds(unsigned char value){

    return rotateRight(value, 1);
}

//prints integer to the display
void displayInteger(int value){

    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    unsigned char* digit_left = NULL;
    unsigned char* digit_right = NULL;
    if(value < 0 || value > 99){ //invalid input
        digit_right = getBytesArray(9); 
        digit_left = getBytesArray(9);
    }
    else
    {   
        
        int digit  = 0 ;
        for (int i = 0; i < 2; i++) {
            digit = value % 10; //extract the digit

            if(i == 0){ //right digit
                //display
                digit_right = getBytesArray(digit);     //get the byte array for right digit
                printf("%d \n", digit);
            }
            else{  //left digit
                //display with shift
                digit_left = getBytesArray(digit);      //get the byte array for left digit
                printf("%d \n", digit);
            }   
            value /= 10;
        }     
    }

    int j= 0; //iterator for the byte arrays
        
    for (int i = 0x0E; i >= 0x02; i-=2) { // 0X0E, 0X0G,.....0X02, 0X00(last second row) (Register Addresses)
        writeI2cReg(i2cFileDesc, i, mapBitsToLeds(digit_left[j]) + rotateRight(mapBitsToLeds(digit_right[j]),4)); // shift by 4 for right digit
        j++;
        // sleepForMs(500);
    }
    //free dynamically allocated memory
    free(digit_left);
    free(digit_right);

    writeI2cReg(i2cFileDesc, 0x00, 0b00000000); //no decimal point for the last row in the display


}

//prints the double value to the display
void displayDouble(double value){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    unsigned char* digit_left = NULL;
    unsigned char* digit_right = NULL;
    if(value < 0.0 || value > 9.9){
        digit_right = getBytesArray(9); 
        digit_left = getBytesArray(9);
    }
    else{


        int integerDigit = (int) value;
        double fractionalPart = value -integerDigit;
        int fractionalDigit = (int) round((fractionalPart*10)); 

        //get the byte array for each digit
        digit_right = getBytesArray(fractionalDigit); 
        digit_left = getBytesArray(integerDigit);
        
        
        printf("double %d %d \n", integerDigit, fractionalDigit );
    }

    int j= 0; //iterator for the byte arrays

    for (int i = 0x0E; i >= 0x02; i-=2) { // 0X0E, 0X0G,.....0X02, 0X00(last second row) (Register Addresses)
        writeI2cReg(i2cFileDesc, i, mapBitsToLeds(digit_left[j]) + rotateRight(mapBitsToLeds(digit_right[j]),4));
        j++;
        // sleepForMs(500);
    }
    free(digit_left);
    free(digit_right);
    writeI2cReg(i2cFileDesc, 0x00, mapBitsToLeds(0b00010000)); // decimal point in the last row (0x0E)

}

void displayFromArr(int LED_Arr[]){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    int j = 0;
    for(int i = 0x0E; i >= 0x00; i-=2){
        writeI2cReg(i2cFileDesc, i, LED_Arr[j]);
    }
}
/*
//main functions
int main(){
    
    reset_Display();
    sleepForMs(100);
    turnOffDisplay();
    turnOnDisplay();
    displayInteger(197);
    // sleepForMs(2000);
    // reset_Display();

    // int i =0;
    // double x = 7.0;
    // while(i < 10){
        
    //     printf("%f \n", x);
    //     displayDouble(x);
    //     sleepForMs(1500);
    //     x+=0.1;
    //     i++;
    // }
    // displayDouble(11.8);
    turnOffDisplay();
    turnOnDisplay();
    return 0;
}
*/