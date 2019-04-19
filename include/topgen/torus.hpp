/*
 *  Torus.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_TORUS_HPP
#define TOPGEN_TORUS_HPP

#include <list>
#include <math.h>
#include <topgen/topology.hpp>
#include <topgen/min_node.hpp>
#include <topgen/dir_channel.hpp>
#include <topgen/ia.hpp>
#include <topgen/exception.hpp>

namespace topgen {

class CTorus : public topology
{
public:
	CTorus(const int n, const int k1, const int k2, const int k3);
	virtual ~CTorus();

	virtual void RunAll();
        virtual void BuildNetwork();
	virtual void BuildInterconnection();
	virtual void BuildChannels();
	virtual void BuildRoutes();

	virtual void Settings(nodeType node_type);

	virtual int GetNumberOfSwitches();
	virtual int GetNumberOfDestinations();
	virtual int GetNodes();
	virtual inline void SetKary(const int dimension, const int kary);
	virtual inline int GetKary(const int dimension);

	virtual void LoadNetwork();
	virtual void LoadRoutes();

	unsigned int GetSameDimensionOutport(unsigned int node_id, unsigned int link_id);

private:
	inline int _Mod(const int dimension, const int x);
	inline int _ExternalPorts(const int i, const int j);
	inline int _ExternalPorts(const int i, const int j, const int k);
	CDIRNode * _SearchNode(const int identifier);
	int _CalculateInternalPort(CDIRNode * current_node, CDIRNode * dest_node, const int identifier, const int destination, const int node_port);

	int * m_pKary;
	int m_iNcube;
	int m_iNodes;
	int m_iChannels;
	int m_iIdentifiers;

	CDIRNode *** m_pNodes2D;

	/*
	 * Three dimensions: <X,Y,Z> + one virtual dimension for the internal cards
	 * Every node has one real identifier and one virtual identifier.
	 */
	CDIRNode **** m_pNodes3D;

	std::list<CDIRChannel*> * m_pChannelList;

	nodeType m_iNodeType;
	int m_iINicPort;
	int m_iIIntraPort;
};

} // namespace topology

#endif /* TOPGEN_TORUS_HPP */
