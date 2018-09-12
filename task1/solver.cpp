#include "solver.hpp"

double SquareTrinomialSolver::FindDiscriminant(double a, double b, double c) {
    return b * b - 4 * a * c;
}

Answer SquareTrinomialSolver::SolveLinearEquation(double k, double b) {
    if (k == 0) {
        if(b == 0) {
            return Answer{0, 0, INF_ROOTS};
        } else {
            return Answer{0, 0, NO_ROOTS};
        }
    }
    double root = -b / k;
    return Answer{root, 0, ONE_ROOT};
}

Answer SquareTrinomialSolver::SolveSquareTrinomial(double a, double b, double c) {
    if (a == 0) {
        return SolveLinearEquation(b, c);
    }
    double discriminant = FindDiscriminant(a, b, c);
    if (discriminant < 0) {
        return Answer{0, 0, NO_ROOTS};
    }
    double x1, x2;
    x1 = (-b + sqrt(discriminant)) / (2 * a);
    x2 = (-b - sqrt(discriminant)) / (2 * a);
    return Answer{x1, x2, TWO_ROOTS};
}

void SquareTrinomialSolver::PrintAnswer(const Answer& answer) {
    switch (answer.code) {
        case ONE_ROOT:
            printf("Root: x = %lf", answer.x1);
            break;
        case TWO_ROOTS:
            printf("Roots: x1 = %lf, x2 = %lf", answer.x1, answer.x2);
            break;
        case INF_ROOTS:
            printf("Infinite number of roots");
            break;
        case NO_ROOTS:
            printf("No roots");
            break;
        default:
            printf("Error occured!");
            break;
    }
}
