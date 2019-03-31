#include <stdio.h>

void array_degenerate_to_ptr(char buf[128])
{
    printf("%li\n", sizeof(buf));
}

int main(void)
{
    char buf[128];
    array_degenerate_to_ptr(buf);
    return 0;
}
