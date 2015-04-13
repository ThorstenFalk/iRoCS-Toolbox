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
**   $Revision: 674 $$Name$
**       $Date: 2005-03-29 20:04:36 +0200 (Tue, 29 Mar 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/03/29 18:04:36  ronneber
**  - added requirement for updateKernelCache() and clearKernelCache()
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef REQUIRESVMFORCROSSVALIDATION_HH
#define REQUIRESVMFORCROSSVALIDATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include "RequireHelpers.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that SVM provides a typedef for a ModelType, a train(
   *  SVM_Problem, ModelType) 
   */
  /*======================================================================*/
  template<typename FV, typename SVM, typename PROBLEM>
  class RequireSVMForCrossValidation
  {
  public:
    ~RequireSVMForCrossValidation()
          {
            if( false)
            {
              SVM s;
              PROBLEM p;
              s.updateKernelCache(p.FV_begin(), p.FV_end(), svt::DereferencingAccessor());
              s.clearKernelCache();
              
              std::vector<char> leaveOutFlagsByUID;
              
              typename SVM::template Traits<FV>::ModelType model;
              s.train( p, model);
              s.retrainWithLeftOutVectors( p, model, leaveOutFlagsByUID, model);
              
              FV fv;
              s.classify( fv, model);
              
              
            }
          }
  };
}


#endif
