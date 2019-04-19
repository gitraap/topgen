/*
 *  DFly.h
 *
 *  Created by Juan Villar and Jesus Escudero-Sahuquillo on 01/11/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#ifndef DFLY_H_
#define DFLY_H_

#include <stdlib.h>
#include <math.h>
#include <list>
#include <topgen/topology.hpp>
#include <topgen/dfly_node.hpp>
#include <topgen/dfly_channel.hpp>
#include <topgen/exception.hpp>

namespace topgen {

class CDFlyNode;
class CDFlyChannel;

class CDFlyGroup
{
public:
  CDFlyGroup(int id, int a, int h, int p);
  virtual ~CDFlyGroup();

  int GetGroupID();
  int GetLocalNodesNumber();
  int GetGlobalLinksNumber();
  int GetGroupNetPortsNumber();
  CDFlyNode * GetGroupNode(int id);

  virtual std::list<CDFlyChannel*> * GetGroupChannels();
  void ConnectLocalPorts();
  void ConnectGlobalPort(int global_src_port, CDFlyGroup * dst_group, int global_dst_port);
  void Dump();

private:

  CDFlyNode * _GetNodeByIdentifier(int identifier);

  int m_iGroupID;
  int m_iNodes;
  int m_iGlobalLinks;
  int m_iNetPortLinks;
  CDFlyNode ** m_pLocalNodesArray;
  std::list<CDFlyChannel*> * m_pGroupChannelList;
};

class CDFlyRoutingAlgorithm;

class CDFly : public topology
{
public:
  
  CDFly(int a, int h, int p, connectionType connection_type, rtAlgorithmType rt_alg);
  virtual ~CDFly();

  virtual void RunAll();
  virtual void BuildNetwork();
  virtual void BuildInterconnection();
  virtual void BuildChannels();
  virtual void BuildRoutes();
  
  virtual int GetNumberOfSwitches();
  virtual int GetNetPorts();
  virtual int GetGroups();
  virtual int GetSwitchesPerGroup();
  virtual int GetGlobalLinksPerSwitch();
  virtual int GetNodesPerSwitch();

  int GetGroupFromNetPort(int net_port);
  int GetGroupFromNode(int node_id);

  int GetNodeInGroupFromNetPort(int net_port);
  int GetNodeInGroupFromNode(int node_id);

  int GetOutputport(int node_id, int dest);
  
  virtual void LoadNetwork();
  virtual void LoadRoutes();
  
private:
  int m_iNetPorts;
  
  int m_iNodes;
  int m_iNodePorts;
  int m_iNodesPerGroup; // a
  int m_iGlobalLinksPerNode; // h
  int m_iNetPortsPerNode; // p
  
  int m_iGroups;
  CDFlyGroup ** m_pGroupsArray;
  connectionType m_iConnectionType;

  CDFlyRoutingAlgorithm * m_pRoutingAlgorithm;
  rtAlgorithmType m_iRtAlgorithmType;
};

class CDFlyRoutingAlgorithm
{
public:
  CDFlyRoutingAlgorithm(rtAlgorithmType rt_alg, CDFly * network);
  virtual ~CDFlyRoutingAlgorithm();

  int DeterministicRouting(int local_node_id, int dest_port_id);

private:
  CDFly * m_pNetwork;
  rtAlgorithmType m_iRtAlgorithmType;
  int m_iSwitchesPerGroup; // a
  int m_iGlobalLinksPerNode; // h
  int m_iNetPortsPerNode; // p
};

} // namespace topology

#endif
