#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "./headers/social/predefined.h"


// Global variables
prop **relational_map = NULL;
bool **visited = NULL;

#define MAX_STACK_SIZE (GRID_SIZE * GRID_SIZE)

// Function declarations
void initialize_map(void);
void initialize_boolean(void);
void calculate_and_print_statistics(const char* label);
void save_to_csv(const char* filename);
extern void __propagate_state__(pos cursor, float news_political_coordinate);
extern void propagate(void);

void initialize_map(void) {
    // Define row thresholds for income-based residential areas.
    // Adjust these percentages for desired population distribution.
    // Poor (lean left):      rows 0   to ~15% of grid height.
    // Working class (slight left/moderate): rows ~15% to ~50% of grid height.
    // Middle class (slight right/moderate): rows ~50% to ~85% of grid height.
    // Rich (lean right):      rows ~85% to 100% of grid height.
    int poor_area_end_row = (int)(0.15f * GRID_SIZE);
    int working_area_end_row = (int)(0.50f * GRID_SIZE);
    int middle_area_end_row = (int)(0.85f * GRID_SIZE);

    // Political coordinate ranges for each income group (0.0=left, 1.0=right)
    float poor_min = 0.05f; float poor_max = 0.35f;
    float working_min = 0.20f; float working_max = 0.50f;
    float middle_min = 0.50f; float middle_max = 0.80f;
    float rich_min = 0.65f; float rich_max = 0.95f;

    // Propagation rates for each income group.
    // Higher rate means opinions spread more easily.
    float poor_propagate_rate = 0.15f;
    float working_propagate_rate = 0.12f;
    float middle_propagate_rate = 0.09f;
    float rich_propagate_rate = 0.06f;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            float random_value = (float)rand() / RAND_MAX; // Get random float between 0.0 and 1.0

            // Assign political coordinate and propagation rate based on residential income area
            if (i < poor_area_end_row) { // Poor residential area
                relational_map[i][j].political_coordinate = poor_min + random_value * (poor_max - poor_min);
                relational_map[i][j].propagate_rate = poor_propagate_rate;
            } else if (i < working_area_end_row) { // Working class area
                relational_map[i][j].political_coordinate = working_min + random_value * (working_max - working_min);
                relational_map[i][j].propagate_rate = working_propagate_rate;
            } else if (i < middle_area_end_row) { // Middle class area
                relational_map[i][j].political_coordinate = middle_min + random_value * (middle_max - middle_min);
                relational_map[i][j].propagate_rate = middle_propagate_rate;
            } else { // Rich residential area
                relational_map[i][j].political_coordinate = rich_min + random_value * (rich_max - rich_min);
                relational_map[i][j].propagate_rate = rich_propagate_rate;
            }

            // Ensure coordinate is within 0.0 and 1.0
            relational_map[i][j].political_coordinate = fmax(0.0f, fmin(1.0f, relational_map[i][j].political_coordinate));
        }
    }
}

// Reset the visited array to false for all cells
void initialize_boolean(void) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            visited[i][j] = false;
        }
    }
}

// Calculate and print average political coordinate over the map
void calculate_and_print_statistics(const char* label) {
    float sum = 0.0f;
    int count = GRID_SIZE * GRID_SIZE;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            sum += relational_map[i][j].political_coordinate;
        }
    }
    printf("[%s] Average political coordinate: %.4f\n", label, sum / count);
}

// Save the political coordinates map to a CSV file
void save_to_csv(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for CSV output");
        return;
    }
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fprintf(fp, "%.4f", relational_map[i][j].political_coordinate);
            if (j < GRID_SIZE - 1) fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}


int main(void) {
    srand((unsigned int)time(NULL));

    // Allocate memory for relational_map rows
    relational_map = (prop **)malloc(sizeof(prop *) * GRID_SIZE);
    if (!relational_map) {
        perror("Failed to allocate memory for relational_map rows");
        return EXIT_FAILURE;
    }

    // Allocate memory for visited rows
    visited = (bool **)malloc(sizeof(bool *) * GRID_SIZE);
    if (!visited) {
        perror("Failed to allocate memory for visited rows");
        free(relational_map);
        return EXIT_FAILURE;
    }

    // Allocate columns for each row
    for (int i = 0; i < GRID_SIZE; i++) {
        relational_map[i] = (prop *)malloc(sizeof(prop) * GRID_SIZE);
        if (!relational_map[i]) {
            perror("Failed to allocate memory for relational_map columns");
            for (int j = 0; j < i; j++) free(relational_map[j]);
            free(relational_map);
            for (int j = 0; j < GRID_SIZE; j++) free(visited[j]);
            free(visited);
            return EXIT_FAILURE;
        }

        visited[i] = (bool *)malloc(sizeof(bool) * GRID_SIZE);
        if (!visited[i]) {
            perror("Failed to allocate memory for visited columns");
            for (int j = 0; j < i; j++) free(visited[j]);
            free(visited);
            for (int j = 0; j <= i; j++) free(relational_map[j]);
            free(relational_map);
            return EXIT_FAILURE;
        }
    }

    // Initialize map and visited arrays
    initialize_map();
    initialize_boolean();

    // Print initial stats and save
    calculate_and_print_statistics("Initial");
    save_to_csv("origin.csv");

    pos current_pos;
    float news_coord;

    // Propagation simulation loop
    for (int cycle = 0; cycle < PROPAGATION_CYCLES; cycle++) {
        printf("--- Propagation Cycle %d ---\n", cycle + 1);


        // Define row thresholds for income-based residential areas (consistent with initialize_map).
        // These define the "origin" areas for news with specific biases.
        int poor_area_end_row_news = (int)(0.15f * GRID_SIZE);
        int working_area_end_row_news = (int)(0.50f * GRID_SIZE);
        int middle_area_end_row_news = (int)(0.85f * GRID_SIZE);
        
        // Adjusted political coordinate ranges for news content based on origin area.
        // Bias is less extreme, allowing more overlap and moderation.
        float news_poor_min = 0.15f; float news_poor_max = 0.45f;   // News from Poor areas: more moderate left
        float news_working_min = 0.30f; float news_working_max = 0.60f; // News from Working class areas: balanced to slightly left
        float news_middle_min = 0.40f; float news_middle_max = 0.70f; // News from Middle class areas: balanced to slightly right
        float news_rich_min = 0.55f; float news_rich_max = 0.85f;   // News from Rich areas: more moderate right
        
        
        // Fake news propagation
        for (int i = 0; i < NUM_FAKE_NEWS_SOURCES; i++) {
            initialize_boolean(); // External function call
        
            current_pos.x = rand() % GRID_SIZE; // Column index
            current_pos.y = rand() % GRID_SIZE; // Row index
        
            float random_bias_for_news = (float)rand() / RAND_MAX; // Random factor for news content variation
        
            // Determine news content (political_coordinate) based on the social class
            // of the randomly chosen news source's origin area (current_pos.y for row).
            if (current_pos.y < poor_area_end_row_news) { // News from Poor area
                news_coord = news_poor_min + random_bias_for_news * (news_poor_max - news_poor_min);
            } else if (current_pos.y < working_area_end_row_news) { // News from Working class area
                news_coord = news_working_min + random_bias_for_news * (news_working_max - news_working_min);
            } else if (current_pos.y < middle_area_end_row_news) { // News from Middle class area
                news_coord = news_middle_min + random_bias_for_news * (news_middle_max - news_middle_min);
            } else { // News from Rich area
                news_coord = news_rich_min + random_bias_for_news * (news_rich_max - news_rich_min);
            }
        
            // Ensure news_coord is within valid range [0.0, 1.0]
            news_coord = fmax(0.0f, fmin(1.0f, news_coord));
        
            // Access relational_map using [row][column] convention: relational_map[y][x]
            relational_map[current_pos.y][current_pos.x].political_coordinate = news_coord;
            relational_map[current_pos.y][current_pos.x].propagate_rate = 0.8f; // High propagation rate for news source
            __propagate_state__(current_pos, news_coord * 0.8f); // Propagate news with scaled effect
        }
        
        // Real news propagation
        for (int i = 0; i < NUM_REAL_NEWS_SOURCES; i++) {
            initialize_boolean(); // External function call
        
            current_pos.x = rand() % GRID_SIZE; // Column index
            current_pos.y = rand() % GRID_SIZE; // Row index
        
            float random_bias_for_news = (float)rand() / RAND_MAX; // Random factor for news content variation
        
            // Determine news content (political_coordinate) based on the social class
            // of the randomly chosen news source's origin area (current_pos.y for row).
            if (current_pos.y < poor_area_end_row_news) { // News from Poor area
                news_coord = news_poor_min + random_bias_for_news * (news_poor_max - news_poor_min);
            } else if (current_pos.y < working_area_end_row_news) { // News from Working class area
                news_coord = news_working_min + random_bias_for_news * (news_working_max - news_working_min);
            } else if (current_pos.y < middle_area_end_row_news) { // News from Middle class area
                news_coord = news_middle_min + random_bias_for_news * (news_middle_max - news_middle_min);
            } else { // News from Rich area
                news_coord = news_rich_min + random_bias_for_news * (news_rich_max - news_rich_min);
            }
        
            // Ensure news_coord is within valid range [0.0, 1.0]
            news_coord = fmax(0.0f, fmin(1.0f, news_coord));
        
            // Access relational_map using [row][column] convention: relational_map[y][x]
            relational_map[current_pos.y][current_pos.x].political_coordinate = news_coord;
            relational_map[current_pos.y][current_pos.x].propagate_rate = 0.8f; // High propagation rate for news source
            __propagate_state__(current_pos, news_coord * 0.8f); // Propagate news with scaled effect
            initialize_boolean(); // Additional external function call
        }
    

        for(int x = 0; x < GRID_SIZE; x++) {
            for(int y = 0; y < GRID_SIZE; y++) {
                __propagate_state__(current_pos, relational_map[x][y].political_coordinate * relational_map[x][y].propagate_rate);
            }
        }
        calculate_and_print_statistics("Final");
        char filename[64];
        sprintf(filename, "results/political_coordinates-%i.csv", cycle + 1);
        save_to_csv(filename);
    }

    // Final stats and save

    // Free allocated memory
    for (int i = 0; i < GRID_SIZE; i++) {
        free(relational_map[i]);
        free(visited[i]);
    }
    free(relational_map);
    free(visited);

    return EXIT_SUCCESS;
}

