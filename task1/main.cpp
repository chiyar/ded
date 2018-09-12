#include <stdio.h>
#include <math.h>
#include <cassert>

void greetUser() {
    printf("# This is square trinomial solver\n"
                   "# Enter coefficients in such order: a b c\n");
}

void readCoefficients(double *a, double *b, double *c) {
    scanf("%lf%lf%lf", a, b, c);
}

double findDiscriminant(double a, double b, double c) {
    return b * b - 4 * a * c;
}

void solveLinearEquation(double k, double b) {
    if (k == 0) {
        if(b == 0) {
            printf("An infinite number of roots");
        } else {
            printf("No roots");
        }
        return;
    }
    double root = -b / k;
    printf("Root: %lf", root);
}

void solveSquareTrinomial(double a, double b, double c) {
    assert(x1 != NULL);
    assert(x2 != NULL);
    assert(x1 != x2);

    if (a == 0) {
        solveLinearEquation(b, c);
        return;
    }
    double discriminant = findDiscriminant(a, b, c);
    if (discriminant < 0) {
        printf("No roots");
        return;
    }
    double x1, x2;
    x1 = (-b + sqrt(discriminant)) / (2 * a);
    x2 = (-b - sqrt(discriminant)) / (2 * a);
    printf("Roots: x1 = %lf, x2 = %lf", x1, x2);
    return;
}

void sayGoodbye() {
    printf("\n# Thank you for using!\n"
                   "# Authored by Yaroslav Chizh\n");
}

int main() {
    greetUser();
    double a, b, c;
    readCoefficients(&a, &b, &c);
    solveSquareTrinomial(a, b, c);
    sayGoodbye();
    return 0;
}
