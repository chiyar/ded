#include <cassert>
#include <string>

#include "solver.hpp"

void Test(const std::string& testName, double a, double b, double c, const Answer& answer) {
    printf("--%s\n", testName.data());
    SquareTrinomialSolver solver;
    assert(answer == solver.SolveSquareTrinomial(a, b, c));
    printf("--Test passed!\n\n");
}

void RunTests() {
    Test("one_root_test", 0, 4.2, 5.3, Answer{-5.3 / 4.2, 0, ONE_ROOT});
    Test("two_roots_test", 1, -2, 1, Answer{1, 1, TWO_ROOTS});
    Test("no_roots_test", 2.1, 2.67, 1.04, Answer{0, 0, NO_ROOTS});
    Test("infinite_number_of_roots_test", 0, 0, 0, Answer{0, 0, INF_ROOTS});
}

int main() {
    RunTests();
    printf("All tests passed!\n");
    return 0;
}
