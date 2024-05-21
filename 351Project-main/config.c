#include "config.h"
#include "pinMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define in 0
#define out 1

//Helper functions
static void runCommand(char* command);
static void echoNumToFile(char* filePath, int num);
static void echoStringToFile(char* filePath, const char* str);
static void sleepForMs(long long delayInMs);

void configManyPins(struct Pin pinArray[], int size, int in_or_out){
    for (int i = 0; i < size; i++){
        configPin(pinArray[i].header_num, pinArray[i].pin_num, in_or_out);
    }
}

void configPin(int header_num, int pin_num, int in_or_out){
    const char* inORoutStr;
    //in == 0 out = 1
    if(in_or_out == in){
        inORoutStr = "in";
    } else if (in_or_out == out){
        inORoutStr = "out";
    } else {
        printf("ERROR INOROUT");
        return;
    }
    char configCommand[100];
    char path[100];

    int GPIO = getGPIO(header_num, pin_num);
    sprintf(configCommand, "config-pin p%d.%d gpio", header_num, pin_num);
    runCommand(configCommand);
    sleepForMs(300);
    echoNumToFile("/sys/class/gpio/export", GPIO);
    sleepForMs(300);
    sprintf(path, "/sys/class/gpio/gpio%d/direction", GPIO);
    echoStringToFile(path, inORoutStr);
    sleepForMs(300);
}

static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL){
            break;
            // printf("--> %s", buffer); // Uncomment for debugging
        }
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
    perror("Unable to execute command:");
    printf(" command: %s\n", command);
    printf(" exit code: %d\n", exitCode);
    }
}

static void echoNumToFile(char* filePath, int num)
{
    // Use fopen() to open the file for write access.
    FILE *pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "%d", num);
    // Close the file using fclose():
    fclose(pFile);
    sleepForMs(300);
}

static void echoStringToFile(char* filePath, const char* str)
{
    // Use fopen() to open the file for write access.
    FILE *pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "%s", str);
    // Close the file using fclose():
    fclose(pFile);
    sleepForMs(300);
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