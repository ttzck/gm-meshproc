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
#include <Eigen/Sparse>

using namespace pmp;

// define some shortcuts for Eigen matrix types
typedef Eigen::SparseMatrix<double> SparseMatrix;
typedef Eigen::MatrixXd DenseMatrix;
typedef Eigen::Triplet<double> Triplet;

//=============================================================================

/// compute triangle area
Scalar area(const SurfaceMesh& mesh, Face f);

/// compute (barycentric) Voronoi area of vertex v
Scalar area(const SurfaceMesh& mesh, Vertex v);

/// compute cotangent weight of edge e
Scalar cotan(const SurfaceMesh& mesh, Edge e);

/// compute cotan mass matrix
void setup_mass_matrix(const SurfaceMesh& mesh, SparseMatrix& M);

/// compute cotan stiffness matrix
void setup_stiffness_matrix(const SurfaceMesh& mesh, SparseMatrix& S);

/// compute two selector matrices that do / don't satisfy boolean criterion
void setup_selector_matrices(const SurfaceMesh &mesh, 
                             VertexProperty<bool> is_selected, 
                             SparseMatrix& Sin, 
                             SparseMatrix& Sout);

//=============================================================================
