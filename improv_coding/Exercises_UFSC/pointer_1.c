#include <stdio.h>

int main(void)
{
    int y = 0;
    int x;
    int *p = &y;

    x = *p;  // x gets the current value of y, which is 0.
    x = 4;   // x is updated to 4.

    (*p)++;  // The value pointed by p is incremented by 1. Since p points to y, y becomes 1.
    x--;     // x is decremented by 1, so x becomes 3.
    *p += x; // The value pointed by p is increased by x. Now y becomes 1 + 3 = 4.

    printf("y = %d\n", y); // Print the final value of y, which is 4.
    return 0;
}