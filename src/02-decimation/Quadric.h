//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================
#pragma once
//=============================================================================

#include "pmp/Types.h"
using namespace pmp;

//=============================================================================

//! Simple class for implementing error quadrics
class Quadric
{
public:

    //! constructor quadric from given plane equation: ax+by+cz+d=0
    Quadric(Scalar a=0.0, Scalar b=0.0, Scalar c=0.0, Scalar d=0.0)
        :  a_(a*a), b_(a*b), c_(a*c), d_(a*d),
                    e_(b*b), f_(b*c), g_(b*d),
                             h_(c*c), i_(c*d),
                                      j_(d*d)
    {}

    //! construct from point and normal specifying a plane
    Quadric(const Normal& n, const Point& p)
    {
        *this = Quadric(n[0], n[1], n[2], -dot(n,p));
    }

    //! add given quadric to this quadric: this += q
    Quadric& operator+=(const Quadric& q)
    {
        a_ += q.a_; b_ += q.b_; c_ += q.c_; d_ += q.d_;
                    e_ += q.e_; f_ += q.f_; g_ += q.g_;
                                h_ += q.h_; i_ += q.i_;
                                            j_ += q.j_;
        return *this;
    }

    //! add two quadrics: q1 + q2
    friend Quadric operator+(Quadric q1, const Quadric& q2)
    {
        q1 += q2;
        return q1;
    }

    //! evaluate quadric Q at position p by computing (p^T * Q * p)
    Scalar operator()(const Point& p) const
    {
        const Scalar x(p[0]), y(p[1]), z(p[2]);
        return a_*x*x + 2.0*b_*x*y + 2.0*c_*x*z + 2.0*d_*x
            +  e_*y*y + 2.0*f_*y*z + 2.0*g_*y
            +  h_*z*z + 2.0*i_*z
            +  j_;
    }

    //! (try to) find point that minimizes quadric error
    Point minimizer() const
    {
        mat3 A{a_, b_, c_, b_, e_, f_, c_, f_, h_};
        vec3 b{d_, g_, i_};
        return -(inverse(A) * b);
    }

private:

    Scalar a_, b_, c_, d_,
               e_, f_, g_,
                   h_, i_,
                       j_;
};


//============================================================================
