#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main() {
    srand(time(NULL));
    float a = (float)(rand()%100) / 100.0f;
    printf("%f\n", a);
}
