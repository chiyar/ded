#include "solver.hpp"

//! Compares number with 0 to the accuracy of epsilon
//!
//! @param [in] number   a number to compare
//! @param [in] epsilon  precision
//!
//! @return True if number is almost equal to 0
bool IsZero(double number, double epsilon) {
    return std::abs(number) < epsilon;
}

bool Answer::operator==(const Answer& other) const {
    return (IsZero(x1 - other.x1, 0.0001)
            && IsZero(x2 - other.x2, 0.0001)
            && code == other.code);
}   

double SquareTrinomialSolver::FindDiscriminant(double a, double b, double c) {
    return b * b - 4 * a * c;
}

Answer SquareTrinomialSolver::SolveLinearEquation(double k, double b) {
    if (IsZero(k, 0.0001)) {
        if(IsZero(b, 0.0001)) {
            return Answer{0, 0, INF_ROOTS};
        } else {
            return Answer{0, 0, NO_ROOTS};
        }
    }
    double root = -b / k;
    return Answer{root, 0, ONE_ROOT};
}

Answer SquareTrinomialSolver::SolveSquareTrinomial(double a, double b, double c) {
    if (IsZero(a, 0.0001)) {
        return SolveLinearEquation(b, c);
    }
    double discriminant = FindDiscriminant(a, b, c);
    if (discriminant < 0) {
        return Answer{0, 0, NO_ROOTS};
    }
    double x1 = (-b + sqrt(discriminant)) / (2 * a);
    double x2 = (-b - sqrt(discriminant)) / (2 * a);
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
