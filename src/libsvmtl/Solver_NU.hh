/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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

/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef SOLVER_NU_HH
#define SOLVER_NU_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SolutionInfo.hh"
#include "Solver.hh"

namespace svt
{
  //
  // Solver for nu-svm classification and regression
  //
  // additional constraint: e^T \alpha = constant
  //
  template< typename FV, typename KF>
  class Solver_NU : public Solver<FV,KF>
  {
  public:
    Solver_NU() {}
    void Solve(int l, const Kernel<FV,KF>& Q, const double *b, const schar *y,
               double *alpha, double Cp, double Cn, double eps,
               SolutionInfo* si, int shrinking,
               ProgressReporter* progressReporter)
          {
            this->si = si;
            Solver<FV,KF>::Solve(l,Q,b,y,alpha,Cp,Cn,eps,si,shrinking,
                                 progressReporter);
          }
  private:
    SolutionInfo *si;
    int select_working_set(int &i, int &j);
    double calculate_rho();
    void do_shrinking();
  };
}

#include "Solver_NU.icc"

#endif
