#pragma once

#include <stdio.h>
#include <math.h>

enum AnswerCode {
    ONE_ROOT,
    TWO_ROOTS,
    INF_ROOTS,
    NO_ROOTS
};

struct Answer {
    double x1, x2; 
    AnswerCode code;
    
    bool operator==(const Answer& other) const {
        return (x1 == other.x1
                && x2 == other.x2
                && code == other.code);
    }
}; 

class SquareTrinomialSolver{
public:
    static Answer SolveSquareTrinomial(double a, double b, double c);
    static void PrintAnswer(const Answer& answer);

private:
    static double FindDiscriminant(double a, double b, double c);
    static Answer SolveLinearEquation(double k, double b);
};
