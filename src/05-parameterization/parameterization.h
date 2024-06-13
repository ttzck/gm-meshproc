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

/// @brief Map the (one and only) boundary loop of the mesh to the unit circle
/// @param mesh The mesh to be parameterized
/// @return Returns false if number of boundary loops != 1
bool parameterize_boundary(SurfaceMesh& mesh);

/// @brief Iteratively compute discrete harmonic parameterization. First call parameterize_boundary().
/// @param mesh The mesh to be parameterized (only one boundary loop allowed)
/// @param use_uniform_laplace Whether to use uniform Laplace (or cotan Laplace otherwise)
/// @param n Number of iterations to perform
void parameterize_iterative(SurfaceMesh& mesh, bool use_uniform_laplace, unsigned int n);

/// @brief Directly compute discrete harmonic parameterization. First call parameterize_boundary().
/// @param mesh The mesh to be parameterized (only one boundary loop allowed)
void parameterize_direct(SurfaceMesh& mesh);

//=============================================================================
