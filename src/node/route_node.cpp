/*
 *  Route_Node.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/node/route_node.hpp"

namespace topgen {

CRoute_Node::CRoute_Node(int identifier, int destination, int number, int output) {
	m_iNodeID = identifier;
	m_iDestination = destination;
	m_iNumber = number;
	m_iOutputPort = output;
}

CRoute_Node::~CRoute_Node() {

}
}  // namespace topology
