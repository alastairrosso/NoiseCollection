#include <iostream>
#include <random>
#include <stdio.h>

#include "glm/glm.hpp"

#define REGION_SIZE 10
#define GRID_LEN_REGS 5
#define GRID_LEN_GRADS (GRID_LEN_REGS+1)
#define GRID_LEN ((REGION_SIZE)*(GRID_LEN_REGS))
#define G(x, y, size) ((y)*(size) + (x))

using namespace glm;
using namespace std;

char vtoa(float brightness) {
    char asc[] = ".;?&&@@@";
    int discr = (int) floor(abs(brightness * 10.0f));
    if (discr > 10) return '@';
    return asc[discr];
}

float smoothStep(float t) {
    return t*t*t*(t*(t*(6)-15) + 10);
}

void genRegion(vector<float>& grid, vector<vec2>& grads, int pos_00x, int pos_00y) {
    float RECIP_REGION_SIZE = 1.0f / REGION_SIZE;

    vec2 g_00 = grads[G(pos_00x,   pos_00y,   GRID_LEN_GRADS)];
    vec2 g_01 = grads[G(pos_00x,   pos_00y+1, GRID_LEN_GRADS)];
    vec2 g_10 = grads[G(pos_00x+1, pos_00y,   GRID_LEN_GRADS)];
    vec2 g_11 = grads[G(pos_00x+1, pos_00y+1, GRID_LEN_GRADS)];

    for (int iv = 0; iv < REGION_SIZE; ++iv) {
        for (int iu = 0; iu < REGION_SIZE; ++iu) {
            float u = iu * RECIP_REGION_SIZE;
            float v = iv * RECIP_REGION_SIZE;

            vec2 r_00 = vec2(u,   v  );
            vec2 r_01 = vec2(u,   v-1);
            vec2 r_10 = vec2(u-1, v  );
            vec2 r_11 = vec2(u-1, v-1);

            float n_00 = dot(g_00, r_00);
            float n_01 = dot(g_01, r_01);
            float n_10 = dot(g_10, r_10);
            float n_11 = dot(g_11, r_11);

            float n_x0 = n_00*smoothStep(1-u) + n_10*smoothStep(u);
            float n_x1 = n_01*smoothStep(1-u) + n_11*smoothStep(u);
            float n_xy = n_x0*smoothStep(1-v) + n_x1*smoothStep(v);

            int grid_x = pos_00x*REGION_SIZE + iu;
            int grid_y = pos_00y*REGION_SIZE + iv;
            grid[G(grid_x, grid_y, GRID_LEN)] = n_xy;
        }
    }
}

int main() {
    const int GRAD_FIELD_SIZE = GRID_LEN_GRADS * GRID_LEN_GRADS;
    vector<float> grid(GRID_LEN * GRID_LEN);
    vector<vec2> grads(GRAD_FIELD_SIZE);

    // sample gradient vectors from fixed set of 8 directions
    // no hash function - just RNG
    vector<vec2> directions = {
        vec2(-1, 1), vec2(0, 1), vec2(1, 1),
        vec2(-1, 0), vec2(1, 0),
        vec2(-1, -1), vec2(0, -1), vec2(1, -1)
    };
    mt19937 rng;
    rng.seed(time(0));
    uniform_real_distribution<> dis(0, 8);
    for (int i = 0; i < GRAD_FIELD_SIZE; ++i) {
        grads[i] = directions[(int) floor(dis(rng))];
    }

    // generate noise for each region
    for (int g_y = 0; g_y < GRID_LEN_REGS; ++g_y) {
        for (int g_x = 0; g_x < GRID_LEN_REGS; ++g_x) {
            genRegion(grid, grads, g_x, g_y);
        }
    }

    // print noise grid
    for (int y = GRID_LEN-1; y >= 0; --y) {
        for (int x = 0; x < GRID_LEN; ++x) {
            cout << vtoa(grid[G(x, y, GRID_LEN)]) << " ";
        }
        cout << endl;
    }

    return 0;
}
