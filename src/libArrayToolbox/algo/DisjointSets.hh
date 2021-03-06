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

#ifndef DISJOINTSETS_H_
#define DISJOINTSETS_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Disjoint Set Data Structure
// Author: Emil Stefanov
// Date: 03/28/06
// Implementaton is as described in http://en.wikipedia.org/wiki/Disjoint-set_data_structure

#include <vector>

class DisjointSets
{
public:

  // Create an empty DisjointSets data structure
  DisjointSets();
  // Create a DisjointSets data structure with a specified number of elements (with element id's from 0 to count-1)
  DisjointSets(int count);
  // Copy constructor
  DisjointSets(const DisjointSets & s);
  // Destructor
  ~DisjointSets();

  // Find the set identifier that an element currently belongs to.
  // Note: some internal data is modified for optimization even though this method is consant.
  int
  FindSet(int element) const;
  // Combine two sets into one. All elements in those two sets will share the same set id that can be gotten using FindSet.
  void
  Union(int setId1, int setId2);
  // Add a specified number of elements to the DisjointSets data structure. The element id's of the new elements are numbered
  // consequitively starting with the first never-before-used elementId.
  void
  AddElements(int numToAdd);
  // Returns the number of elements currently in the DisjointSets data structure.
  int
  NumElements() const;
  // Returns the number of sets currently in the DisjointSets data structure.
  int
  NumSets() const;

  // to renumber the sets, necessary for connected component labelling
  void
  ReNum();

private:

  // Internal Node data structure used for representing an element
  struct Node
  {
    int rank; // This roughly represent the max height of the node in its subtree
    int index; // The index of the element the node represents
    Node* parent; // The parent node of the node
  };

  int m_numElements; // the number of elements currently in the DisjointSets data structure.
  int m_numSets; // the number of sets currently in the DisjointSets data structure.
  std::vector<Node*> m_nodes; // the list of nodes representing the elements
};

#endif /* DISJOINTSETS_H_ */
