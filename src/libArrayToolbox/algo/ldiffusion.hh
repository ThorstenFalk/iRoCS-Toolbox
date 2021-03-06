/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/*
 * ldiffusion.h
 *
 *  Created on: Oct 12, 2010
 *      Author: kun
 */

#ifndef LDIFFUSION_H_
#define LDIFFUSION_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "lParallel.hh"

#include <blitz/array.h>

int mindex3(int x, int y, int z, int sizx, int sizy);

template<typename Type, int Dim>
void anisotropicDiffusion3D_explicit(
    blitz::Array<Type,Dim> &image, blitz::Array<Type,Dim> &Dxx,
    blitz::Array<Type,Dim> &Dyy, blitz::Array<Type,Dim> &Dzz,
    blitz::Array<Type,Dim> &Dxy, blitz::Array<Type,Dim> &Dxz,
    blitz::Array<Type,Dim> &Dyz, double delta_t = 0.0625, int v = 1);

double CalculateDiffusionNewGreyValue(
    double *u, double *a, double *b, double *c, double *d, double *e,
    double *f, int x, int y, int z, int nx, int ny, int nz, int px, int py,
    int pz, double dt, int *dimsu);

template<typename GType>
void get1DAOS_blitz(
    blitz::Array<GType,1>& p, double delta_t, int len, int m,
    double* alpha, double *beta, double *gamma, double* L, double *M,
    double *R);

template<typename Type, typename gType>
void AOS_3D_blitz(
    const blitz::Array<Type, 3>& u, blitz::Array<gType, 3>& g,
    blitz::Array<Type, 3>& u_new, const double delta_t);

template<typename Type>
void nonlinearDiffusion3D_AOS(
    blitz::Array<Type, 3>& image,
    const blitz::TinyVector<double, 3>& spacing, const int num_iter = 4,
    const double kappa = 0.1, const double delta_t = 1, const int mode = 0,
    const int v = 1);

void tridiagonal_Thomas_decomposition(
    double* alpha, double* beta, double* gamma,
    double* l, double* m, double* r, unsigned long N);

void tridiagonal_Thomas_solution(
    double* l, double* m, double* r, double* d, double* y, unsigned long N);

template<typename Type>
void tridiagonal_Thomas_solution_blitz(
    double* l, double* m, double* r, blitz::Array<Type,1> &d, double* y,
    unsigned long N);

#include "ldiffusion.icc"

#endif /* LDIFFUSION_H_ */
