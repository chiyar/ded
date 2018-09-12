#include "solver.hpp"

void simple_test() {
    double a = 1, b = -2, c = 1;
    Answer answer{2, 1, TWO_ROOTS};

    SquareTrinomialSolver solver;
    assert(answer == solver.SolveSquareTrinomial(a, b, c));
}

int main() {
    simple_test();
    return 0;
}
