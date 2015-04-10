/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

#ifndef ATBHOUGHTRANSFORM_HH
#define ATBHOUGHTRANSFORM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>

#include <blitz/array.h>

#include "Array.hh"
#include "ATBDataSynthesis.hh"
#include "CentralGradientFilter.hh"
#include "SeparableConvolutionFilter.hh"

#include <libProgressReporter/ProgressReporter.hh>

namespace atb
{

/*======================================================================*/
/*! 
 *   Fast implementation of the spherical hough transform for
 *   non-overlapping spheres
 *
 *   \param data          The raw gray values to search spherical structures
 *                        in
 *   \param houghmaps     In this vector the two houghmaps for each gradient
 *                        direction are returned
 *   \param houghmapsR    The corresponding radius maps are returned here
 *   \param rMin          Minimum radius
 *   \param rMax          Maximum radius
 *   \param rStep         Radius increment
 *   \param preSmoothing  standard deviation of the gaussian derivative that
 *                        is used as derivative operator
 *   \param postSmoothing The houghmaps will be smoothed with a gaussian of
 *                        the standard deviation given here to regularize
 *                        the result and reduce spurious detections
 *   \param minMagnitude  Only gradient magnitudes above the threshold
 *                        provided may vote
 */
/*======================================================================*/
  template<typename DataT>
  void computeHoughTransform(
      const blitz::Array<DataT,3>& data,
      std::vector< blitz::Array<double,3>* >& houghmaps,
      std::vector< blitz::Array<double,3>* >& houghmapsR,
      const double rMin, const double rMax,
      const double rStep, const double preSmoothing,
      const double postSmoothing, const double minMagnitude);

/*======================================================================*/
/*! 
 *   Fast implementation of the spherical hough transform for
 *   non-overlapping spheres
 *
 *   \param data          The raw gray values to search spherical structures
 *                        in
 *   \param houghmaps     In this vector the two houghmaps for each gradient
 *                        direction are returned
 *   \param houghmapsR    The corresponding radius maps are returned here
 *   \param rMin          Minimum radius
 *   \param rMax          Maximum radius
 *   \param rStep         Radius increment
 *   \param preSmoothing  standard deviation of the gaussian derivative that
 *                        is used as derivative operator
 *   \param postSmoothing The houghmaps will be smoothed with a gaussian of
 *                        the standard deviation given here to regularize
 *                        the result and reduce spurious detections
 *   \param minMagnitude  Only gradient magnitudes above the threshold
 *                        provided may vote
 */
/*======================================================================*/
  template<typename DataT>
  void computeHoughTransform(
      const Array<DataT,3>& data,
      std::vector< Array<double,3>* >& houghmaps,
      std::vector< Array<double,3>* >& houghmapsR,
      const double rMin, const double rMax,
      const double rStep, const double preSmoothing,
      const double postSmoothing, const double minMagnitude);

/*======================================================================*/
/*! 
 *   Compute spherical hough transform using Gradient voting.
 *
 *   \param data The raw gray values to search spherical structures in
 *   \param response The magnitude of the hough response
 *   \param radiusUm The corresponding structure radii in micrometers
 *   \param radiusRangeUm The radius search range in micrometers
 *   \param radiusStepUm The radius search step in micrometers
 *   \param minMagnitude Only gradients with magnitudes greater than this
 *     threshold may cast votes
 *   \param invertGradients If given the gradients cast votes in the opposite
 *     direction resulting in high responses for dark spheres on bright
 *     background
 *   \param pr If given progress is reported using this progress reporter
 */
/*======================================================================*/
  void houghTransform(
      Array<float,3> const &data, Array<float,3> &response,
      Array<float,3> &radiusUm,
      blitz::TinyVector<double,2> const &radiusRangeUm, double radiusStepUm,
      double minMagnitude = 0.0, bool invertGradients = false,
      iRoCS::ProgressReporter *pr = NULL);

}

#include "HoughTransform.icc"

#endif
