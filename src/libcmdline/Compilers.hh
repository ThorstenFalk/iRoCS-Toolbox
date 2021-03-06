/**************************************************************************
 *
 * Copyright (C) 2005-2015 Olaf Ronneberger, Jörg Mechnich, Florian Pigorsch,
 *                         Mario Emmenlauer, Thorsten Schmidt
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
**       Title: compilers for various datatypes
**    $RCSfile$
**   $Revision: 16 $$Name$
**       $Date: 2002-03-26 08:36:28 +0100 (Tue, 26 Mar 2002) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#ifndef COMPILERS_HH
#define COMPILERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cctype>
#include <cstdlib>
#include <iostream>

#include "Compiler.hh"

template<> 
bool 
Compiler<int>::compile(
    const char* arg,
    const char*& endptr,
    int& value)
{
  if (arg==0) // null string - nothing to do
  {
    return true; 
  } 
  else if (*arg==0) // empty string
  {
    setErrorMsg("empty integer value specified");
    return false;
  }
  
  // compile the string into an integer
  char* ptr;
  int result=strtol(arg, &ptr, 0);  
  
  if (ptr==arg) // first char is invalid
  {
    setErrorMsg("invalid integer value \""+std::string(arg)+"\"");
    return false;
  }
  
  endptr=ptr;
  value=(int)result;
  return true;
};

template<> 
bool 
Compiler<double>::compile(
    const char* arg,
    const char*& endptr,
    double& value)
{
  if (arg==0) // null string - nothing to do
  {
    return true; 
  } 
  else if (*arg==0) // empty string
  {
    setErrorMsg("empty integer value specified");
    return false;
  }
  
  // compile the string into an integer
  char* ptr;
  double result=strtod(arg, &ptr);  
  
  if (ptr==arg) // first char is invalid
  {
    setErrorMsg("invalid double value \""+std::string(arg)+"\"");
    return false;
  }
  
  endptr=ptr;
  value=result;
  return true;
};

template<> 
bool 
Compiler<std::string>::compile(
    const char* arg,
    const char*& endptr,
    std::string& value)
{
  if (arg!=0)
  {
    endptr=0;
    value=arg;
  } 

  return true;
}; 

template<> 
bool 
Compiler<bool>::compile(
    const char* arg,
    const char*& endptr,
    bool& value)
{
  if (arg==0) // no argument -> true
  {
    value=true;
    return true; 
  } 
  else if (*arg==0) // empty string -> true
  {
    value=true;
    return true;
  }

  std::string s(arg); 
  for (std::string::iterator p=s.begin();
       p!=s.end();
       ++p)
  {
    *p=tolower(*p);
  }
  
  if (s=="1" || s=="on" || s=="true")
  {
    endptr=0;
    value=true;
    return true;
  }
  else if (s=="0" || s=="off" || s=="false")
  {
    endptr=0;
    value=false;
    return true;
  }
  
  return false;
};

#endif
