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

#include "FloatChannelTreeWidgetItem.hh"

#include <libMultiChannelModel/FloatChannel.hh>
#include "MultichannelModelTreeWidget.hh"

namespace iRoCS {

  FloatChannelTreeWidgetItem::FloatChannelTreeWidgetItem(
    FloatChannel &channel, MultichannelModelTreeWidget *treeWidget)
    : ChannelTreeWidgetItem(channel, treeWidget) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelTreeWidgetItem@" << this
      << "::FloatChannelTreeWidgetItem channel=\"" << &channel
      << "\" treeWidget=\"" << &treeWidget << "\">" << std::endl;
#endif
    setText(2, "32-Bit float");
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatChannelTreeWidgetItem@" << this
      << "::FloatChannelTreeWidgetItem>" << std::endl;
#endif    
  }

  FloatChannelTreeWidgetItem::~FloatChannelTreeWidgetItem() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelTreeWidgetItem@" << this
      << "::~FloatChannelTreeWidgetItem />" << std::endl;
#endif
  }

  void FloatChannelTreeWidgetItem::updateElementSizeUm() {}

  void FloatChannelTreeWidgetItem::updateTransformation() {}

  void FloatChannelTreeWidgetItem::updateData() {}

  FloatChannelTreeWidgetItem *FloatChannelTreeWidgetItem::instance(
    FloatChannel &channel, MultichannelModelTreeWidget *treeWidget) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelTreeWidgetItem::instance channel=\"" << &channel
      << "\" treeWidget=\"" << treeWidget << "\">" << std::endl;
#endif
    // First check whether an Observer for this channel/tree combination
    // already exists. If so return the pointer to the existing Observer.
    for (std::set<ChannelObserver*>::const_iterator it =
      channel.observers().begin(); it != channel.observers().end(); ++it) {
      FloatChannelTreeWidgetItem *item =
        dynamic_cast<FloatChannelTreeWidgetItem*>(*it);
      if (item != NULL && item->treeWidget() == treeWidget) {
#ifdef DEBUG_VERBOSE_XML
        std::cerr << "return " << item << " (old instance)"
          << std::endl;
        std::cerr << "</FloatChannelTreeWidgetItem::instance>" << std::endl;
#endif
        return item;
      }
    }
    FloatChannelTreeWidgetItem *item =
      new FloatChannelTreeWidgetItem(channel, treeWidget);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "return " << item << " (new instance)" << std::endl;
    std::cerr << "</FloatChannelTreeWidgetItem::instance>" << std::endl;
#endif
    return item;
  }

}