#include <string.h>
#include <stdio.h>

#ifndef TEST_H
#define TEST_H

struct Pin
{
    int header_num;
    int pin_num;
};


void configPin(int header_num, int pin_num, int in_or_out);
void configManyPins(struct Pin array[], int size, int in_or_out); //in = 0, out = 1

#endif