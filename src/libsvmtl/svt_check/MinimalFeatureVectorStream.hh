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
**   Copyright: GPL $Author: ronneber $
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
#ifndef MINIMALFEATUREVECTORSTREAM_HH
#define MINIMALFEATUREVECTORSTREAM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>
#include "RequireHelpers.hh"
#include "MinimalFeatureVector.hh"

namespace svt_check
{
  class MinimalFeatureVectorStream : public MinimalFeatureVector
  {
  public:
    MinimalFeatureVectorStream() {}
    
  private:
    MinimalFeatureVectorStream( const MinimalFeatureVectorStream& x) {}
    void operator=( const MinimalFeatureVectorStream& x) {}
    
  };

  inline
  std::ostream& operator<<( std::ostream& os, const MinimalFeatureVectorStream& f)
  {
    return os;
  }
  
  inline
  std::istream& operator>>(std::istream& is, MinimalFeatureVectorStream& fv)
  {
    return is;
  }
  
}


#endif
