/*
 *  Route_Node.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_ROUTE_NODE_HPP
#define TOPGEN_ROUTE_NODE_HPP

#include <stdio.h>

namespace topgen {

class CRoute_Node {
public:
	CRoute_Node(int identifier, int destination, int number, int output);
	virtual ~CRoute_Node();

	int m_iNodeID;
	int m_iDestination;
	int m_iNumber;
	int m_iOutputPort;
};

} // namespace topology

#endif /* TOPGEN_ROUTE_NODE_HPP */
