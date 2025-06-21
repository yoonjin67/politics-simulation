#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "../headers/social/predefined.h"

int main() {
    srand(time(NULL));
    float r = 0;
    for(int i = 0; i < 100; i++, printf("------TEST[%d]------\nr = %f\n", i, r)) {
        r = (float)(rand()%100) / 100.0f;
        for(int j = 0; j < 30; j++) {
            printf("----------[EPOCH %dj----------\n", j);
        float another_state = (float)(rand()%100) / 100.0f;
            float appliance = calculate_appliance(another_state);
            printf("r1 = %f\n", another_state);
            printf("appliance = %f\napplication = %f\n", appliance, r + appliance);
        }
    }
    for(int j = 0; j < 30; j++) {
        printf("----------[EPOCH %dj----------\n", j);
        float another_state = (float)(rand()%100) / 100.0f;
        float appliance = calculate_appliance(another_state);
            printf("r1 = %f\n", another_state);
        printf("appliance = %f\napplication = %f\n", appliance, r + appliance);
    }

}
