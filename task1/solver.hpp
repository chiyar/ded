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
    
    bool operator==(const Answer& other) const;
};

//! Provides an interface to work with square trinomial
class SquareTrinomialSolver{
public:
    //! Finds roots of square trinomial ax2 + bx + c
    //!
    //! @param [in] a a-coefficient
    //! @param [in] b b-coefficient
    //! @param [in] c c-coefficient
    //!
    //! @return Roots and number of them
    //!
    //! @note In case of no roots or infinity number of roots
    //!       returns NO_ROOTS or INF_ROOTS flag respectively
    static Answer SolveSquareTrinomial(double a, double b, double c);

    //! Prints roots of square trinomial
    //! @param answer 
    static void PrintAnswer(const Answer& answer);

private:
    //! Finds discriminant of square trinomial ax2 + bx + c
    //! 
    //! @param [in] a a-coefficient
    //! @param [in] b b-coefficient
    //! @param [in] c c-coefficient
    //!
    //! @return Discriminant of square trinomial
    static double FindDiscriminant(double a, double b, double c);

    //! Find root of linear function kx + b
    //!
    //! @param [in] k k-coefficient
    //! @param [in] b b-coefficient
    //!
    //! @return Root of linear function
    //!
    //! @note In case of no roots or infinity number of roots
    //!       returns NO_ROOTS or INF_ROOTS flag respectively
    static Answer SolveLinearEquation(double k, double b);
};
