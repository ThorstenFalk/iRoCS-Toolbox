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

#ifndef ATTACHIROCSSCTTOCELLSEGMENTATIONTHREAD_HH
#define ATTACHIROCSSCTTOCELLSEGMENTATIONTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

#include "AttachIRoCSSCTToCellSegmentationParametersDialog.hh"

class AttachIRoCSSCTToCellSegmentationThread : public QThread
{

  Q_OBJECT

  public:
  
  AttachIRoCSSCTToCellSegmentationThread(
      AttachIRoCSSCTToCellSegmentationParametersDialog const &parameters,
      ShellCoordinateTransform *sct, iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL, QWidget* parent = NULL);
  ~AttachIRoCSSCTToCellSegmentationThread();

  void run();

private:

  AttachIRoCSSCTToCellSegmentationParametersDialog const &_parameters;
  ShellCoordinateTransform *p_sct;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget *p_mainWidget;

};

#endif
