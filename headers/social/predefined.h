#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define GRID_SIZE 60
#define NUM_FAKE_NEWS_SOURCES 2
#define NUM_REAL_NEWS_SOURCES 6
#define PROPAGATION_CYCLES 10000

typedef struct _pos {
    int x;
    int y;
} pos;

#define max(a,b)        \
({                      \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;  \
})

#define min(a,b)        \
({                      \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;  \
})

#define calculate_appliance(x) \
max(min(1, (0.5f - (x)) * APPLYING_PROP), -1.0f)

#define rise_propagate_rate(x) \
min((x) * 1.01f, 1.0f)

typedef struct _prop {
    float propagate_rate;
    float political_coordinate;
} prop;

extern prop **relational_map;
extern bool **visited;
