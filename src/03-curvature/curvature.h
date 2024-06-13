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

/// compute mean curvature for all (non-boundary) vertices
void compute_mean_curvature(SurfaceMesh& _mesh);

/// compute Gauss curvature for all (non-boundary) vertices
void compute_gauss_curvature(SurfaceMesh& _mesh);

/** Convert curvature values to texture coordinates.
 * U-coordinate should be between 0 and 1,
 * V-coordinate should be 0.
 * If the texture is a 1D color ramp, we get nice color coding */
void curvature_to_texture_coordinates(SurfaceMesh& _mesh);

//=============================================================================
