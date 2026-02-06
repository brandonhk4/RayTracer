#ifndef PERLIN_H
#define PERLIN_H

class perlin {
    private:
        static const int point_count = 256;
        vec3 randvec[point_count];
        int perm_x[point_count];
        int perm_y[point_count];
        int perm_z[point_count];

        static void perlin_generate_perm(int* p) {
            for (int i = 0; i < point_count; ++i) {
                p[i] = i;
            }
            permute(p, point_count);
        }

        static void permute(int* p, int n) {
            for (int i = n - 1; i > 0; --i) {
                int target = random_int(0, i);
                p[i] ^= p[target] ^= p[i] ^= p[target];
            }
        }

        static float perlin_interp(const vec3 cube[2][2][2], float u, float v, float w) {
            // Hermitian smoothing
            float uu = u * u * (3 - 2 * u);
            float vv = v * v * (3 - 2 * v);
            float ww = w * w * (3 - 2 * w);
            float accum = 0.0;

            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    for (int k = 0; k < 2; ++k) {
                        vec3 weight_v(u - i, v - j, w - k);
                        accum += (i * uu + (1 - i) * (1 - uu)) *
                                 (j * vv + (1 - j) * (1 - vv)) *
                                 (k * ww + (1 - k) * (1 - ww)) *
                                 dot(cube[i][j][k], weight_v);
                    }
                }
            }
            return accum;
        }
    
    public:
        perlin() {
            for (int i = 0; i < point_count; ++i) {
                randvec[i] = vec3::random(-1, 1).dir();
                // randfloat[i] = random_float();
            }

            perlin_generate_perm(perm_x);
            perlin_generate_perm(perm_y);
            perlin_generate_perm(perm_z);
        }

        // returns a value [-1, 1]
        double noise(const vec3& p) const {
            float u = p.x - std::floor(p.x);
            float v = p.y - std::floor(p.y);
            float w = p.z - std::floor(p.z);

            int i = int(std::floor(p.x));
            int j = int(std::floor(p.y));
            int k = int(std::floor(p.z));
            vec3 cube[2][2][2];
            
            for (int di = 0; di < 2; ++di) {
                for (int dj = 0; dj < 2; ++dj) {
                    for (int dk = 0; dk < 2; ++dk) {
                        cube[di][dj][dk] = randvec[
                            perm_x[(i + di) & 255] ^
                            perm_y[(j + dj) & 255] ^
                            perm_z[(k + dk) & 255]
                        ];
                    }
                }    
            }

            return perlin_interp(cube, u, v, w);
        }

        double turb(const vec3& p, int depth) const {
            float accum = 0.0;
            vec3 temp = p;
            float weight = 1.0;

            for (int i = 0; i < depth; ++i) {
                accum += weight * noise(temp);
                weight *= 0.5;
                temp *= 2;
            }

            return std::fabs(accum);
        }
};

#endif