#include <limits.h>
#include <stdio.h>

int square(int x)
{
    return x * x;
}

int main(void)
{
    long long value;
    int x;

    printf("Enter a number to calculate its square: ");
    if (scanf("%lld", &value) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    if (value < INT_MIN || value > INT_MAX) {
        printf("Invalid value. Please enter an integer within the range [%d, %d].\n", INT_MIN, INT_MAX);
        return 1;
    }

    x = (int)value;
    printf("The square of %d is %d.\n", x, square(x));
    return 0;
}