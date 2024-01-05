#include <qb.h>

float rot = 0;

// -----------------------------------------------------------------------------

// Псевдослучайный шум
double rand (double x, double y) {

    double m = sin(x * 12.9898 + y * 78.233) * 43758.54531229988;
    return m - floor(m);
}

// Просто шум в точке
double noise(float x, float y) {

    double ix = floor(x);
    double iy = floor(y);
    double fx = x - ix;
    double fy = y - iy;

    double a = rand(ix,      iy);
    double b = rand(ix + 1., iy);
    double c = rand(ix,      iy + 1.);
    double d = rand(ix + 1., iy + 1.);

    double ux = fx * fx * (3 - 2 * fx);
    double uy = fy * fy * (3 - 2 * fy);

    // a b Интерполяция по 4-м точкам
    // c d Используя fx, fy в качестве позиции fx,fy=[0,1]
    return a * (1 - ux) +
           b * ux +
           (c - a) * uy * (1 - ux) +
           (d - b) * ux * uy;
}

// Фрактальный шум
double fbm (double x, double y) {

    double value  = 0;
    double amp    = .5;
    double freq   = 0;

    for (int i = 0; i < 5; i++) {

        value += amp * noise(x, y);
        x = 2.*x;
        y = 2.*y;
        amp = amp * .5;
    }

    return value;
}

// -----------------------------------------------------------------------------

// Нормализация вектора
vec3 normalize (vec3 c) {

    double d = sqrt(c.x*c.x + c.y*c.y + c.z*c.z);

    d   = d ?: 1;
    c.x = c.x / d;
    c.y = c.y / d;
    c.z = c.z / d;

    return c;
};

// Вычисление пересечений луча D со сферой в точке O и радиусом R
double sphere (vec3 d, vec3 o, double r) {

    double sp = -1;

    double a = d.x * d.x + d.y * d.y + d.z * d.z;
    double b = -2 * (d.x * o.x + d.y * o.y + d.z * o.z);
    double c = o.x * o.x + o.y * o.y + o.z * o.z - r;
    double det = b*b - 4*a*c;

    if (det >= 0) {

        det = sqrt(det);
        double x1 = (-b - det) / (2 * a);
        double x2 = (-b + det) / (2 * a);

        if (x1 < 0 && x2 < 0) sp = -1;
        if (x1 < 0 && x2 > 0) sp = x2;
        if (x1 > 0 && x2 < 0) sp = x1;
        if (x1 > 0 && x2 > 0 && x1 < x2) sp = x1;
        if (x1 > 0 && x2 > 0 && x1 >= x2) sp = x2;
    }

    return sp;
}

// Кастомная палитра
void custompal() {

    int i;

    // Earth landscape [0..63]
    FOR(i,1,63) palette(   i, rgb(   i, 128 + i * 2, 4*i));

    // Earth water [64..71]
    FOR(i,0,7)  palette(64+i, rgb(i * 32, 64 + 24 * i, 255));

    // Starfeld [72..135]
    FOR(i,0,63) palette(72+i, rgb(4*i, 4*i, 4*i));
}

// -----------------------------------------------------------------------------

program(13) custompal(); fps {

    vec3 c, o = {0, 0, 1.5}, sun = normalize({1, 0, -.5});

    double u, v, m, dt = 8;

    for (int y=  -100; y < 100; y++)
    for (int x = -160; x < 160; x++) {

        c = {(float)x / 100, (float)y / 100, 1};
        m = sphere(c, o, 1);

        if (m > 0) {

            // Normal vector
            c.x = c.x * m - o.x;
            c.y = c.y * m - o.y;
            c.z = c.z * m - o.z;
            c = normalize(c);

            // UV-calc
            u = atan2(c.z, c.x);
            v = atan2(c.z, c.y);
            u = u + rot;

            // Get fractional part
            u = u - floor(u);
            v = v - floor(v);
            m = fbm(dt * u, dt * v) * 63;

            // Directional Light
            // dl = c.x * sun.x + c.y * sun.y + c.z * sun.z
            // IF dl < 0 THEN dl = 0

            // Water or surface
            m = m < 32 ? 64 : (2*m - 64);

            pset(160 + x, 100 - y, m);
        }
    }

    rot += 0.005;

} end

