#include <iostream>
#include <random>
#include <stdio.h>

#include "glm/glm.hpp"

#define REG_LEN 10
#define REG_PER_ROW 5
#define GRAD_PER_ROW (REG_PER_ROW+1)
#define G(x, y, size) (y)*(size) + (x)

using namespace glm;
using namespace std;

char vtoa(float brightness) {
    char asc[] = ".;;??&&&@@@";
    int discr = (int) floor(abs(brightness * 10.0f));
    if (discr > 10) return '@';
    return asc[discr];
}

float smoothStep(float t) {
    return t*t*t*(t*(t*(6)-15) + 10);
}

void genRegion(vector<float>& grid, vector<vec2>& grads, vec2 pos_g_00) {
    float rec_reg_size = 1.0f / REG_LEN;

    vec2 g_00 = grads[G(pos_g_00.x, pos_g_00.y, GRAD_PER_ROW)];
    vec2 g_01 = grads[G(pos_g_00.x, pos_g_00.y+1, GRAD_PER_ROW)];
    vec2 g_10 = grads[G(pos_g_00.x+1, pos_g_00.y, GRAD_PER_ROW)];
    vec2 g_11 = grads[G(pos_g_00.x+1, pos_g_00.y+1, GRAD_PER_ROW)];

    for (int iv = 0; iv < REG_LEN; ++iv) {
        for (int iu = 0; iu < REG_LEN; ++iu) {
            float u = iu * rec_reg_size;
            float v = iv * rec_reg_size;

            vec2 r_00 = vec2(u, v);
            vec2 r_01 = vec2(u, v-1);
            vec2 r_10 = vec2(u-1, v);
            vec2 r_11 = vec2(u-1, v-1);

            float n_00 = dot(g_00, r_00);
            float n_01 = dot(g_01, r_01);
            float n_10 = dot(g_10, r_10);
            float n_11 = dot(g_11, r_11);

            float n_x0 = n_00*smoothStep(1-u) + n_10*smoothStep(u);
            float n_x1 = n_01*smoothStep(1-u) + n_11*smoothStep(u);
            float n_xy = n_x0*smoothStep(1-v) + n_x1*smoothStep(v);

            grid[G(pos_g_00.x*REG_LEN + iu, pos_g_00.y*REG_LEN + iv, REG_LEN*REG_PER_ROW)] = n_xy;
        }
    }
}

int main() {
    int len_grid = REG_LEN*REG_PER_ROW * REG_LEN*REG_PER_ROW;
    int len_grads = GRAD_PER_ROW * GRAD_PER_ROW;
    vector<float> grid(len_grid);
    vector<vec2> grads(len_grads);

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
    for (int i = 0; i < len_grads; ++i) {
        grads[i] = directions[(int) floor(dis(rng))];
    }

    for (int g_y = 0; g_y < REG_PER_ROW; ++g_y) {
        for (int g_x = 0; g_x < REG_PER_ROW; ++g_x) {
            genRegion(grid, grads, vec2(g_x, g_y));
        }
    }

    for (int y = (REG_PER_ROW*REG_LEN)-1; y >= 0; --y) {
        for (int x = 0; x < REG_PER_ROW*REG_LEN; ++x) {
            cout << vtoa(grid[G(x, y, REG_LEN*REG_PER_ROW)]) << " ";
        }
        cout << endl;
    }

    return 0;
}
