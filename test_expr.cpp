#include <stdio.h>
#include <time.h>
#include "expr.h"

int main() {
    struct Test { const char *expr; int err; double result; } tests[] = {
        {"1", -1, 1.0},
        {"1+1", -1, 2.0},
        {"3 * 4", -1, 12.0},
        {"((1))", -1, 1.0},
        {"3*--3", -1, 9.0},
        {"1+2*(3+4)", -1, 15.0},
        {"1+2*(3+4*(5+6.5))", -1, 99.0},
        {"x1", -1, 100.0},
        {"x1+y1*2", -1, 500},
        {"1--1", -1, 2},
        {"1/(8--8)", -1, 0.0625},

        {"1+z2*4", 4, -1},
        {"1+2*", 4, -1},
        {"1+(2*3", 6, -1},
        {"1+2()", 3, -1},
        {"+1", 0, -1},
    };

    std::map<std::string, double> vars;
    vars["x0"] = 3.14;
    vars["y0"] = 2.718;
    vars["x1"] = 100;
    vars["y1"] = 200;

    int errors = 0;
    int ntests = sizeof(tests)/sizeof(tests[0]);
    for (int i=0; i<ntests; i++) {
        const char *s = tests[i].expr;
        double res = -1;
        try {
            Expr expr = Expr::partialParse(s, vars);
            if (*s) throw Expr::Error("Extra characters");
            if (tests[i].err != -1) throw Expr::Error("Parsing should have failed");
            res = expr.eval();
            if (tests[i].result != res) throw Expr::Error("Unexpected result");
        } catch (Expr::Error& err) {
            if (tests[i].err == s - tests[i].expr) {
                // Ok; parsing error position is correct
            } else {
                errors++;
                printf("TEST FAILED: \"%s\" (err=%i, result=%0.3f) --> res=%0.3f\n%s\n\n",
                       tests[i].expr, tests[i].err, tests[i].result,
                       res, err.what());
            }
        }
    }
    printf("%i errors on %i tests\n", errors, ntests);

    int w=640, h=480;
    vars["k"] = 255.0/((w*w+h*h)/4);
    double& y = vars["y"];
    double& x = vars["x"];
    std::vector<unsigned char> img(w*h);
    Expr e("((x-320)*(x-320) + (y-240)*(y-240))*k", vars);
    clock_t start = clock();
    int i = 0;
    for (y=0; y<h; y++) {
        for (x=0; x<w; x++) {
            img[i++] = e.eval();
        }
    }
    clock_t stop = clock();
    printf("Test image generated in %0.3fms (%.0f pixels/sec)\n",
           (stop - start)*1000.0/CLOCKS_PER_SEC,
           double(w*h)*CLOCKS_PER_SEC/(stop-start+1));
    FILE *f = fopen("test.pgm", "wb");
    if (f) {
        fprintf(f, "P5\n%i %i 255\n", w, h);
        fwrite(&img[0], 1, w*h, f);
        fclose(f);
    } else {
        fprintf(stderr, "Error generating test.pgm\n");
    }

    return errors != 0;
}