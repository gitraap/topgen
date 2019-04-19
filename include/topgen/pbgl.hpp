/*
 *  PBGL.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/04/2019.
 *  Copyright 2019 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_PBGL_HPP
#define TOPGEN_PBGL_HPP

#include <topgen/topology.hpp>
#include <topgen/exception.hpp>
#include <topgen/bgl.hpp>

// Manual selection
//#define TOPGEN_PBGL

namespace topgen {

class CPBGL : public topology
{
public:
	CPBGL();
	virtual ~CPBGL();

	virtual void RunAll();
        virtual void BuildNetwork();
	virtual void BuildRoutes();
	virtual void BuildInterconnection();
	virtual void BuildChannels();

	virtual void LoadNetwork();
	virtual void LoadRoutes();

	virtual int GetNumberOfSwitches();
	virtual int GetNodes();
       
        void run_pbgl(const std::string gvfile, int from, int to);
       
private:     
        void dijkstra_example();
#ifdef TOPGEN_PBGL  
	PGraph m_PGraph;
#endif
        
};

} // namespace topology

#endif /* TOPGEN_PBGL_HPP */
