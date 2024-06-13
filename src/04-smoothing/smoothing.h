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

#include <pmp/SurfaceMesh.h>
using namespace pmp;

//=============================================================================

/// @brief perform Laplacian smoothing (explicit integration)
/// @param mesh The mesh to be smoothed
/// @param iters Number of iterations
/// @param use_uniform_laplace Whether to use uniform or cotangent Laplace
void explicit_smoothing(SurfaceMesh& mesh,
                        unsigned int iters = 10,
                        bool use_uniform_laplace = false);

/// @brief perform Laplacian smoothing (implicit integration)
/// @param mesh The mesh to be smoothed
/// @param timestep The time step
void implicit_smoothing(SurfaceMesh& mesh,
                        Scalar timestep = 0.001);

//=============================================================================
