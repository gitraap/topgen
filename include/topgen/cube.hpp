/*
 *  Cube.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_CUBE_HPP
#define TOPGEN_CUBE_HPP

#include <math.h>
#include <list>
#include <boost/dynamic_bitset.hpp>
#include <topgen/topology.hpp>
#include <topgen/dir_node.hpp>
#include <topgen/dir_channel.hpp>
#include <topgen/exception.hpp>

namespace topgen {

class CCube : public topology
{
public:
	CCube(int dimension);
	virtual ~CCube();

	virtual void RunAll();
        virtual void BuildNetwork();
	virtual void BuildInterconnection();
	virtual void BuildChannels();
	virtual void BuildRoutes();

	virtual int GetNumberOfSwitches();
	virtual int GetNodes();

	virtual void LoadNetwork();
	virtual void LoadRoutes();

private:
	CDIRNode * _GetNodeByIdentifier(int identifier);

	/* Hack: port and dimension are synonyms, except net port */

	int m_iDimension;
	int m_iNodes;
	int m_iChannels;
	int m_iIdentifiers;
	int m_iWordWide;
	CDIRNode ** m_pNodeVector;
	std::list<CDIRChannel*> * m_pChannelList;
};

} // namespace topology

#endif /* TOPGEN_CUBE_HPP */
