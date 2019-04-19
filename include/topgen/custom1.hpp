/*
 *  Custom1.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_CUSTOM1_HPP
#define TOPGEN_CUSTOM1_HPP

#include <list>
#include <topgen/topology.hpp>
#include <topgen/dir_node.hpp>
#include <topgen/dir_channel.hpp>
#include <topgen/exception.hpp>

namespace topgen {

class CCustom1 : public topology
{
public:
	CCustom1();
	virtual ~CCustom1();
        
        virtual void RunAll();
	virtual void BuildNetwork();
	virtual void BuildRoutes();
	virtual void BuildInterconnection();
	virtual void BuildChannels();

	virtual void LoadNetwork();
	virtual void LoadRoutes();
        
	virtual int GetNumberOfSwitches();
	virtual int GetNodes();

private:
	int m_iEndNodes;
	int m_iChannels;
};

} // namespace topology

#endif /* TOPGEN_CUSTOM1_HPP */
