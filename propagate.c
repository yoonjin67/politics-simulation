#include <stdbool.h>
#include <string.h>
#include "./headers/social/predefined.h"

#define MAX_STACK_SIZE (GRID_SIZE * GRID_SIZE)

// extern variables from main
#define E 2.718
float sigmoid(float x) {
    return 1.0f / (1.0f + (float)pow(E, -x));
}
extern prop **relational_map;
extern void initialize_boolean(void);
extern bool **visited;

void __propagate_state__(pos cursor, float news_political_coordinate) {
    pos dfs_stack[MAX_STACK_SIZE];
    int dfs_top = -1;

    dfs_stack[++dfs_top] = cursor;

    while (dfs_top >= 0) {
        pos current = dfs_stack[dfs_top--];

        if (visited[current.x][current.y]) continue;
        visited[current.x][current.y] = true;
        relational_map[current.x][current.y].propagate_rate = sigmoid(fabs(0.5 - relational_map[current.x][current.y].political_coordinate));

        // Accumulate political coordinate by adding news_centered
        float absolute_delta = abs(relational_map[current.x][current.y].political_coordinate - news_political_coordinate);
        if(absolute_delta < 0.4) {
            float similarity = 1.0 - absolute_delta;
            float radicality = fabs(relational_map[current.x][current.y].political_coordinate);
            float bias_strength = 1.0f + 2.0f * radicality + 2.0f * relational_map[current.x][current.y].propagate_rate; //bias rate calculated by propagate rate and radicality
            relational_map[current.x][current.y].political_coordinate *= powf(similarity, bias_strength);
        } else if(abs(relational_map[current.x][current.y].political_coordinate - news_political_coordinate) > 0.5) {
            relational_map[current.x][current.y].political_coordinate *= 1.01;
        }

        // Visit neighbors
        pos neighbors[4] = {
            {current.x + 1, current.y},
            {current.x - 1, current.y},
            {current.x, current.y + 1},
            {current.x, current.y - 1}
        };

        for (int i = 0; i < 4; i++) {
            pos n = neighbors[i];
            if (n.x >= 0 && n.x < GRID_SIZE && n.y >= 0 && n.y < GRID_SIZE) {
                if (!visited[n.x][n.y]) {
                    dfs_stack[++dfs_top] = n;
                }
            }
        }
    }
}


