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

#include "OpenGlRenderingSettingsWidget.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QDial>
#include <QtGui/QPushButton>

#include "StringSelectionControlElement.hh"
#include "ColorControlElement.hh"
#include "BoolControlElement.hh"
#include "DoubleControlElement.hh"
#include "IntControlElement.hh"
#include "ChannelSelectionControlElement.hh"
#include "DoubleRangeControlElement.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "IRoCSChannelSpecs.hh"

OpenGlRenderingSettingsWidget::OpenGlRenderingSettingsWidget(
    OpenGlRenderingViewWidget *view, QWidget *parent,
    Qt::WindowFlags f)
        : QWidget(parent, f), p_view(view)
{
  QVBoxLayout *layout = new QVBoxLayout;

  QHBoxLayout *updateLayout = new QHBoxLayout;

  p_liveUpdateControl = new BoolControlElement(tr("Live update"));
  p_liveUpdateControl->setValue(false);
  connect(p_liveUpdateControl, SIGNAL(valueChanged(bool)),
          SLOT(_setLiveUpdateEnabled(bool)));
  updateLayout->addWidget(p_liveUpdateControl);
  QPushButton *updateButton = new QPushButton(tr("Update"));
  connect(updateButton, SIGNAL(clicked()), SIGNAL(renderingStateChanged()));
  updateLayout->addWidget(updateButton);

  layout->addLayout(updateLayout);

  QGroupBox *renderingGroup = new QGroupBox(tr("Rendering State"));
  QVBoxLayout *renderingLayout = new QVBoxLayout;

  p_backgroundColorControl = new ColorControlElement(tr("Background Color"));
  renderingLayout->addWidget(p_backgroundColorControl);

  QStringList renderingModes;
  renderingModes << "Solid" << "Wireframe" << "Points";

  p_frontFaceRenderingControl = new StringSelectionControlElement(
      tr("Front face rendering"), renderingModes);
  p_frontFaceRenderingControl->setValue(int(Solid));
  renderingLayout->addWidget(p_frontFaceRenderingControl);

  p_backFaceRenderingControl = new StringSelectionControlElement(
      tr("Back face rendering"), renderingModes);
  p_backFaceRenderingControl->setValue(int(Solid));
  renderingLayout->addWidget(p_backFaceRenderingControl);

  p_backFaceCullingControl = new BoolControlElement(tr("Back face culling"));
  p_backFaceCullingControl->setValue(true);
  renderingLayout->addWidget(p_backFaceCullingControl);

  p_lightControl = new BoolControlElement(tr("Light enabled"));
  p_lightControl->setValue(true);
  renderingLayout->addWidget(p_lightControl);

  p_ambientFactorControl = new DoubleControlElement(tr("Ambient factor"));
  p_ambientFactorControl->setRange(0.0, 1.0);
  p_ambientFactorControl->setValue(0.1);
  p_ambientFactorControl->setSingleStep(0.05);
  renderingLayout->addWidget(p_ambientFactorControl);

  p_materialShininessControl = new DoubleControlElement(
      tr("Material shininess"));
  p_materialShininessControl->setRange(0.0, 1000.0);
  p_materialShininessControl->setValue(50.0);
  renderingLayout->addWidget(p_materialShininessControl);

  p_latitudeSamplingControl = new DoubleControlElement(
      tr("Segment length (um)"));
  p_latitudeSamplingControl->setRange(0.1, 1000.0);
  p_latitudeSamplingControl->setValue(10.0);
  renderingLayout->addWidget(p_latitudeSamplingControl);

  p_longitudeSamplingControl = new IntControlElement(
      tr("Radial segments"));
  p_longitudeSamplingControl->setRange(3, 1000);
  p_longitudeSamplingControl->setValue(10);
  renderingLayout->addWidget(p_longitudeSamplingControl);

  renderingGroup->setLayout(renderingLayout);

  layout->addWidget(renderingGroup);

  QGroupBox *clippingGroup = new QGroupBox(tr("Clipping"));
  QVBoxLayout *clippingLayout = new QVBoxLayout;

  p_channelControl = new ChannelSelectionControlElement(
      tr("iRoCS channel"), view->model(),
      ChannelSpecs::IRoCS | ChannelSpecs::IRoCSSCT);
  clippingLayout->addWidget(p_channelControl);

  p_zRangeControl = new DoubleRangeControlElement(tr("z Range"));
  p_zRangeControl->setRange(
      -std::numeric_limits<double>::infinity(),
      std::numeric_limits<double>::infinity());
  p_zRangeControl->setSingleStep(1.0);
  p_zRangeControl->setLowerBound(-10000.0);
  p_zRangeControl->setUpperBound(10000.0);
  p_zRangeControl->setEnabled(p_channelControl->isEnabled());
  connect(p_channelControl, SIGNAL(channelListEmpty(bool)),
          p_zRangeControl, SLOT(setDisabled(bool)));
  clippingLayout->addWidget(p_zRangeControl);

  p_rRangeControl = new DoubleRangeControlElement(tr("r Range"));
  p_rRangeControl->setRange(0.0, std::numeric_limits<double>::infinity());
  p_rRangeControl->setSingleStep(1.0);
  p_rRangeControl->setLowerBound(0.0);
  p_rRangeControl->setUpperBound(10000.0);
  p_rRangeControl->setEnabled(p_channelControl->isEnabled());
  connect(p_channelControl, SIGNAL(channelListEmpty(bool)),
          p_rRangeControl, SLOT(setDisabled(bool)));
  clippingLayout->addWidget(p_rRangeControl);

  QHBoxLayout *phiRangeLayout = new QHBoxLayout;
  QLabel *phiRangeLabel = new QLabel(tr("Phi range"));
  phiRangeLayout->addWidget(phiRangeLabel);
  p_phiMin = new QDial;
  p_phiMin->setRange(0, 359);
  p_phiMin->setSingleStep(1);
  p_phiMin->setValue(0);
  p_phiMin->setWrapping(true);
  p_phiMin->setNotchesVisible(true);
  p_phiMin->setEnabled(p_channelControl->isEnabled());
  connect(p_channelControl, SIGNAL(channelListEmpty(bool)),
          p_phiMin, SLOT(setDisabled(bool)));
  phiRangeLayout->addWidget(p_phiMin);

  phiRangeLayout->addWidget(new QLabel("-"));

  p_phiMax = new QDial;
  p_phiMax->setRange(0, 359);
  p_phiMax->setSingleStep(1);
  p_phiMax->setValue(359);
  p_phiMax->setWrapping(true);
  p_phiMax->setNotchesVisible(true);
  p_phiMax->setEnabled(p_channelControl->isEnabled());
  connect(p_channelControl, SIGNAL(channelListEmpty(bool)),
          p_phiMax, SLOT(setDisabled(bool)));
  phiRangeLayout->addWidget(p_phiMax);

  clippingLayout->addLayout(phiRangeLayout);
  clippingGroup->setLayout(clippingLayout);

  layout->addWidget(clippingGroup);
  setLayout(layout);
}

OpenGlRenderingSettingsWidget::~OpenGlRenderingSettingsWidget()
{}

blitz::TinyVector<unsigned char,3>
OpenGlRenderingSettingsWidget::backgroundColor() const
{
  return p_backgroundColorControl->value();
}

OpenGlRenderingSettingsWidget::RenderingState
OpenGlRenderingSettingsWidget::frontFaceRendering() const
{
  switch (p_frontFaceRenderingControl->value())
  {
  case int(Solid):
    return Solid;
  case int(Wireframe):
    return Wireframe;
  case int(Dots):
    return Dots;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
  }
  return Solid;
}

OpenGlRenderingSettingsWidget::RenderingState
OpenGlRenderingSettingsWidget::backFaceRendering() const
{
  switch (p_backFaceRenderingControl->value())
  {
  case int(Solid):
    return Solid;
  case int(Wireframe):
    return Wireframe;
  case int(Dots):
    return Dots;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
  }
  return Solid;
}

bool OpenGlRenderingSettingsWidget::backFaceCullingEnabled() const
{
  return p_backFaceCullingControl->value();
}

bool OpenGlRenderingSettingsWidget::lightEnabled() const
{
  return p_lightControl->value();
}

float OpenGlRenderingSettingsWidget::ambientFactor() const
{
  return static_cast<float>(p_ambientFactorControl->value());
}

float OpenGlRenderingSettingsWidget::materialShininess() const
{
  return static_cast<float>(p_materialShininessControl->value());
}

double OpenGlRenderingSettingsWidget::latitudeSamplingUm() const
{
  return p_latitudeSamplingControl->value();
}

int OpenGlRenderingSettingsWidget::longitudeSampling() const
{
  return p_longitudeSamplingControl->value();
}

ChannelSpecs *OpenGlRenderingSettingsWidget::clippingChannel() const
{
  return p_channelControl->selectedChannel();
}

double OpenGlRenderingSettingsWidget::zMin() const
{
  return p_zRangeControl->lowerBound();
}

double OpenGlRenderingSettingsWidget::zMax() const
{
  return p_zRangeControl->upperBound();
}

double OpenGlRenderingSettingsWidget::rMin() const
{
  return p_rRangeControl->lowerBound();
}

double OpenGlRenderingSettingsWidget::rMax() const
{
  return p_rRangeControl->upperBound();
}

double OpenGlRenderingSettingsWidget::phiMin() const
{
  return M_PI * static_cast<double>(p_phiMin->value()) / 180.0 - M_PI;
}

double OpenGlRenderingSettingsWidget::phiMax() const
{
  return M_PI * static_cast<double>(p_phiMax->value()) / 180.0 - M_PI;
}

void OpenGlRenderingSettingsWidget::_setLiveUpdateEnabled(bool enable)
{
  if (enable)
  {
    connect(p_backgroundColorControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_frontFaceRenderingControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_backFaceRenderingControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_backFaceCullingControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_lightControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_ambientFactorControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_materialShininessControl, SIGNAL(valueChanged()),
            SIGNAL(renderingStateChanged()));
    connect(p_latitudeSamplingControl, SIGNAL(valueChanged()),
            SIGNAL(clippingStateChanged()));
    connect(p_longitudeSamplingControl, SIGNAL(valueChanged()),
            SIGNAL(clippingStateChanged()));
    connect(p_channelControl, SIGNAL(valueChanged()),
            SIGNAL(clippingStateChanged()));
    connect(p_zRangeControl, SIGNAL(valueChanged()),
            SIGNAL(clippingStateChanged()));
    connect(p_rRangeControl, SIGNAL(valueChanged()),
            SIGNAL(clippingStateChanged()));
    connect(p_phiMin, SIGNAL(valueChanged(int)),
            SIGNAL(clippingStateChanged()));
    connect(p_phiMax, SIGNAL(valueChanged(int)),
            SIGNAL(clippingStateChanged()));
  }
  else
  {
    disconnect(p_backgroundColorControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_frontFaceRenderingControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_backFaceRenderingControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_backFaceCullingControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_lightControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_ambientFactorControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_materialShininessControl, SIGNAL(valueChanged()),
               this, SIGNAL(renderingStateChanged()));
    disconnect(p_latitudeSamplingControl, SIGNAL(valueChanged()),
               this, SIGNAL(clippingStateChanged()));
    disconnect(p_longitudeSamplingControl, SIGNAL(valueChanged()),
               this, SIGNAL(clippingStateChanged()));
    disconnect(p_channelControl, SIGNAL(valueChanged()),
               this, SIGNAL(clippingStateChanged()));
    disconnect(p_zRangeControl, SIGNAL(valueChanged()),
               this, SIGNAL(clippingStateChanged()));
    disconnect(p_rRangeControl, SIGNAL(valueChanged()),
               this, SIGNAL(clippingStateChanged()));
    disconnect(p_phiMin, SIGNAL(valueChanged(int)),
               this, SIGNAL(clippingStateChanged()));
    disconnect(p_phiMax, SIGNAL(valueChanged(int)),
               this, SIGNAL(clippingStateChanged()));
  }
}
