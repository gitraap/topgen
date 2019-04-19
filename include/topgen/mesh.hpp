/*
 *  Mesh.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_MESH_HPP
#define TOPGEN_MESH_HPP

#include <list>
#include <math.h>
#include <topgen/topology.hpp>
#include <topgen/dir_node.hpp>
#include <topgen/dir_channel.hpp>
#include <topgen/exception.hpp>

namespace topgen {

class CMesh : public topology
{
public:
	CMesh(const int dimensions, const int karity1, const int karity2, const int karity3);
	virtual ~CMesh();

	virtual int GetNumberOfSwitches();
	virtual int GetNodes();

	virtual void RunAll();
        virtual void BuildNetwork();
	virtual void BuildRoutes();
	virtual void BuildInterconnection();
	virtual void BuildChannels();

	virtual void LoadNetwork();
	virtual void LoadRoutes();

	virtual void SetKary(const int dimension, const int kary);
	virtual inline int GetKary(const int dimension);
private:
	int _ExternalPorts(const int i, const int j);
	int _ExternalPorts(const int i, const int j, const int k);
	CDIRNode * _SearchNode(const int identifier);

	int * m_pKary;
	int m_iNodes;
	int m_iDimension;
	int m_iChannels;
	int m_iIdentifiers;

	std::list<CDIRChannel*> * m_pChannelList;
	CDIRNode *** m_pNodes2D;
	CDIRNode **** m_pNodes3D;
};

} // namespace topology

#endif /* TOPGEN_MESH_HPP */
