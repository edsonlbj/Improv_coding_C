#include "stdio.h"

int main() 
{
    int y, *p, x; // Declare the variables y and x and the pointer p. All as INT
    y = 0;        // Initialize y with 0
    p = &y;       // Initialize the pointer p with the addres of the variable y
    x = *p;       // Initialize x with the value pointed by p (that is the value of y=0)
    x = 4;        // Change the value of x to 4
    (*p)++;       // Increment the value pointed by p (that is the value of y) by 1. So now y=1
    x--;          // Decrement the value of x by 1. So now x=3
    *p += x;      // Increment the value pointed by p (that is the value of y) by the value of x(that is 3). Now y=4
    printf("y = %d \n", y); //print "y = 4"
    return(0);
}