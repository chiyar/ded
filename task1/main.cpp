#include "solver.hpp"

void GreetUser() {
    printf("# This is square trinomial solver\n"
                   "# Enter coefficients in such order: a b c\n");
}

void ReadCoefficients(double *a, double *b, double *c) {
    scanf("%lf%lf%lf", a, b, c);
}

void SayGoodbye() {
    printf("\n# Thank you for using!\n"
                   "# Authored by Yaroslav Chizh\n");
}

int main() {
    GreetUser();
    double a, b, c;
    ReadCoefficients(&a, &b, &c);
    SquareTrinomialSolver solver;
    Answer answer = solver.SolveSquareTrinomial(a, b, c);
    solver.PrintAnswer(answer);
    SayGoodbye();
    return 0;
}
