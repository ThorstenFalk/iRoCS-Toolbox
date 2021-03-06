/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

#ifndef CYLINDERMARKER_HH
#define CYLINDERMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SphereMarker.hh"

class OrthoViewWidget;
class AnnotationChannelSpecs;
class DoubleControlElement;

class CylinderMarkerPresetWidget : public SphereMarkerPresetWidget 
{

  Q_OBJECT

public:

  CylinderMarkerPresetWidget(
      AnnotationChannelSpecs *channel, QWidget *parent = 0);
  virtual ~CylinderMarkerPresetWidget();
  
  virtual void getValues(Marker *marker) const;
  virtual void setValues(Marker const *marker);
  
protected:
  
  DoubleControlElement *p_lengthControl;
  DoubleControlElement *p_thetaControl, *p_phiControl;

};


class CylinderMarkerControlWidget : public SphereMarkerControlWidget
{

  Q_OBJECT

public:

  CylinderMarkerControlWidget(
      AnnotationChannelSpecs *channel, QWidget *parent = 0);
  virtual ~CylinderMarkerControlWidget();
  
  virtual void getValues(Marker *marker) const;

public slots:

  virtual void setValues(Marker const *marker);
  
protected:
  
  DoubleControlElement *p_lengthControl;
  DoubleControlElement *p_thetaControl, *p_phiControl;  

};


class CylinderMarker : public SphereMarker
{
  
  Q_OBJECT

public:
  
  CylinderMarker(blitz::TinyVector<double,3> const &positionUm = 0.0,
             double radiusUm = 5.0, double lengthUm = 5.0,
             double theta = 0.0, double phi = 0.0,
             AnnotationChannelSpecs *channel = NULL);
  CylinderMarker(CylinderMarker const &marker);
  virtual ~CylinderMarker();

  double lengthUm() const;
  void setLengthUm(double lengthUm);
  
  double theta() const;
  void setTheta(double theta);
  
  double phi() const;
  void setPhi(double phi);
  
  blitz::TinyVector<double,3> orientation() const;
  void setOrientation(blitz::TinyVector<double,3> orientation);

  virtual Marker::MarkerType markerType() const;
  virtual bool inherits(Marker::MarkerType type) const;
  
  virtual MarkerRenderer *addRenderer(ViewWidget* view);

  static void save(
      AnnotationChannelSpecs const *channel,
      BlitzH5File &outFile, std::string const &group,
      iRoCS::ProgressReporter *pr = NULL);
  static void load(
      AnnotationChannelSpecs *channel,
      BlitzH5File const &inFile, std::string const &group,
      iRoCS::ProgressReporter *pr = NULL);

  virtual void writeCSVHeader(std::ofstream &out) const;
  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  virtual bool occupiesPositionUm(
      blitz::TinyVector<double,3> const &positionUm) const;

protected:
  
  virtual void _updateBoundingBox() const;

  double _lengthUm, _theta, _phi;

};

#endif
