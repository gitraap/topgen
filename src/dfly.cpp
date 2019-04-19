/*
 *  DFLy.cpp
 *
 *  Created by Juan Villar and Jesus Escudero-Sahuquillo on 01/11/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include "topgen/dfly.hpp"

namespace topgen {

// CDFlyGroup class implementation
CDFlyGroup::CDFlyGroup(int id, int a, int h, int p)
{
  m_iGroupID = id;
  m_iNodes = a;
  m_iGlobalLinks = h;
  m_iNetPortLinks = p;

  m_pLocalNodesArray = new CDFlyNode * [m_iNodes];
  for (int i = 0; i < m_iNodes; i++){
    m_pLocalNodesArray[i] = new CDFlyNode((m_iGroupID * m_iNodes) + i, a, h, p);
    m_pLocalNodesArray[i]->Init();
  }
  
  m_pGroupChannelList = new std::list<CDFlyChannel*>;  
}

CDFlyGroup::~CDFlyGroup(){
 
  for (int i = 0; i < m_iNodes; i++){
    delete m_pLocalNodesArray[i];
  }
  delete [] m_pLocalNodesArray;

  CDFlyChannel * ch;
  
  while (!m_pGroupChannelList->empty()) {
    ch = m_pGroupChannelList->front();
    m_pGroupChannelList->pop_front();
    delete ch;
  }
  delete m_pGroupChannelList;
}

int CDFlyGroup::GetGroupID(){
  return m_iGroupID;
}

int CDFlyGroup::GetLocalNodesNumber(){
  return m_iNodes;
}

int CDFlyGroup::GetGlobalLinksNumber(){
  return m_iGlobalLinks;
}

int CDFlyGroup::GetGroupNetPortsNumber(){
  return m_iNetPortLinks * m_iNodes;
}

CDFlyNode * CDFlyGroup::GetGroupNode(int id)
{
  return m_pLocalNodesArray[id];
}

std::list<CDFlyChannel*> * CDFlyGroup::GetGroupChannels(){
  return m_pGroupChannelList;
}

void CDFlyGroup::ConnectLocalPorts()
{
  int i, j, port;
  CDFlyNode * src_node;
  CDFlyNode * dst_node;
  dflyNodeStruct_t * src_node_t;
  dflyNodeStruct_t * dst_node_t;

  // Connect the Nodes of the Group (Fully connected only)                                                                                                     
  for (i = 0; i < (m_iNodes - 1); i++){
    src_node = m_pLocalNodesArray[i];
    for (port = i, j = i + 1; j < m_iNodes; port++, j++){
      dst_node = m_pLocalNodesArray[j];
      src_node_t = src_node->GetPortByPort(m_iNetPortLinks + m_iGlobalLinks + port);
      dst_node_t = dst_node->GetPortByPort(m_iNetPortLinks + m_iGlobalLinks + i);
      
      src_node_t->remote_node = dst_node;
      src_node_t->remote_port = dst_node_t;
      
      dst_node_t->remote_node = src_node;
      dst_node_t->remote_port = src_node_t;
    }
  }
}

void CDFlyGroup::ConnectGlobalPort(int global_src_port, CDFlyGroup * dst_group, int global_dst_port)
{
  CDFlyNode * dst_node;
  dflyNodeStruct_t * src_node_t;
  dflyNodeStruct_t * dst_node_t;

  int local_node_id = global_src_port / m_iGlobalLinks;
  int local_node_port = (global_src_port % m_iGlobalLinks) + m_iNetPortLinks;
  int dest_node_id = global_dst_port / m_iGlobalLinks;
  int dest_node_port = (global_dst_port % m_iGlobalLinks) + m_iNetPortLinks;

  char * name = new char[TOPGEN_MAX_STRING_LENGTH];
  snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ConnectGlobalPort -> [SrcGr:%d, GSP: %d, LN:%d, LNP:%d][DstGr:%d, GDP:%d, DN:%d, DNP:%d]\n",
	 m_iGroupID, global_src_port, local_node_id, local_node_port, dst_group->GetGroupID(), global_dst_port, dest_node_id, dest_node_port);
  delete [] name;
  
  dst_node = dst_group->GetGroupNode(dest_node_id);
  dst_node_t = dst_node->GetPortByPort(dest_node_port);
  src_node_t = m_pLocalNodesArray[local_node_id]->GetPortByPort(local_node_port);
  
  src_node_t->remote_node = dst_node;
  src_node_t->remote_port = dst_node_t;

  dst_node_t->remote_node = m_pLocalNodesArray[local_node_id];
  dst_node_t->remote_port = src_node_t;
}

void CDFlyGroup::Dump()
{
  //TODO Implement method CDFlyGroup::Dump()
}

// --------------------------
// CDFly class implementation
// --------------------------
 
CDFly::CDFly(int a, int h, int p, connectionType connection_type, rtAlgorithmType rt_alg_type) : topology() {
  m_iNetPorts = a * p * (a * h + 1);
  m_iGroups = (a * h) + 1;  

  m_iNodes = a * m_iGroups;
  m_iNodePorts = p + h + (a - 1);
  m_iNodesPerGroup = a;
  m_iGlobalLinksPerNode = h;
  m_iNetPortsPerNode = p;
  
  m_pGroupsArray = new CDFlyGroup * [m_iGroups];
  for(int i = 0; i < m_iGroups; i++){
    m_pGroupsArray[i] = new CDFlyGroup(i, a, h, p);
  }
  
  m_iConnectionType = connection_type;
  m_pRoutingAlgorithm = NULL;
  m_iRtAlgorithmType = rt_alg_type;
}

CDFly::~CDFly() {
  _freeMemory();
  
  if (m_pRoutingAlgorithm)
      delete m_pRoutingAlgorithm;

  for (int i = 0; i < m_iGroups; i++)
    delete m_pGroupsArray[i];
  delete [] m_pGroupsArray;
}

void CDFly::RunAll() {
  BuildNetwork();
  BuildInterconnection();
  BuildChannels();
  LoadNetwork();
  Vectorize();
  //PreBuildRoutes();
  BuildRoutes();
  LoadRoutes();
  //PostVectorize();
}

void CDFly::BuildNetwork() {
    CDFlyNode::ResetCounter();

#ifdef PRINT_BUILD_NETWORK
        printf("*** Initial configuration *** \n\n");

        for (int i; i < m_iGroups; i++)
	  m_pGroupsArray[i]->Dump();

        printf("********** END ************** \n\n");
#endif
      
}

void CDFly::BuildInterconnection() {
  
  CDFlyGroup * src_group;
  CDFlyGroup * dst_group;

  // Connect the internal groups ports
  for (int i = 0; i < m_iGroups; i++){
    m_pGroupsArray[i]->ConnectLocalPorts();
  }

  // Groups will be fully-connected
  for (int i = 0; i < (m_iGroups - 1); i++) {
    src_group = m_pGroupsArray[i];
    for (int port = i, j = i + 1; j < m_iGroups; port++, j++) {
      dst_group = m_pGroupsArray[j];
      src_group->ConnectGlobalPort(port, dst_group, i);
    }
  }

#ifdef PRINT_BUILD_INTERCONNECTION
  // Print nodes
  for (int i = 0; i < m_iGroups; i++)
    m_pGroupsArray[i]->Dump();

#endif
}

void CDFly::BuildChannels() {
  
  CDFlyChannel * ch;
  CDFlyNode * local_node;
  dflyNodeStruct_t * local_node_port;

  for (int i = 0; i < m_iGroups; i++){
    for (int j = 0; j < m_iNodesPerGroup; j++){
      local_node = m_pGroupsArray[i]->GetGroupNode(j);
      for (int k = 0; k < local_node->GetPorts(); k++){ 
	local_node_port = local_node->GetPortByPort(k);

	// Inter-switch channels
	if (k >= m_iNetPortsPerNode){
	  ch = new CDFlyChannel(m_iChannels++,
				nodeSTD_dfly,
				local_node,
				local_node_port,
				local_node_port->remote_node,
				local_node_port->remote_port);

          if (local_node_port->remote_node == NULL ||
	      local_node_port->remote_port == NULL){
            char * errmsg = new char[256];
            snprintf(errmsg, 256, "Error: Bad channel information");
            throw new CException(errmsg);
          }

	}
	// NetPort Channels
	else if(k < m_iNetPortsPerNode){
	  ch = new CDFlyChannel(m_iChannels++,
				iaSTD_dfly,
				local_node,
				local_node_port);
	}
	
	((std::list<CDFlyChannel*> *)(m_pGroupsArray[i]->GetGroupChannels()))->push_back(ch);
	//m_pChannelList->push_back(ch);
      }
    }
  }
}

void CDFly::BuildRoutes() {
  m_pRoutingAlgorithm = new CDFlyRoutingAlgorithm(m_iRtAlgorithmType, this);
}

int CDFly::GetNumberOfSwitches() {
  return m_iNodes;
}

int CDFly::GetNetPorts() {
  return m_iNetPorts;
}

int CDFly::GetGroups() {
  return m_iGroups;
}

int CDFly::GetSwitchesPerGroup()
{
  return m_iNodesPerGroup;
}

int CDFly::GetGlobalLinksPerSwitch()
{
  return m_iGlobalLinksPerNode;
}

int CDFly::GetNodesPerSwitch()
{
  return m_iNetPortsPerNode;
}

void CDFly::LoadNetwork() {
  char * name;
  CElementType_Node * new_element;
  CDFlyNode * local_node;
  dflyNodeStruct_t * local_node_port;
  std::list<CDFlyChannel*> * group_channel_list;

  for (int i = 0; i < m_iGroups; i++){
    for (int j = 0; j < m_iNodesPerGroup; j++){
      local_node = m_pGroupsArray[i]->GetGroupNode(j);

      // Nodes 
      new_element = new CElementType_Node("cs", local_node->GetIdentifier(), local_node->GetPorts(), 0);
      m_pElements->push_back(new_element);

      // NetPorts
      for (int k = 0; k < m_iNetPortsPerNode; k++){
        local_node_port = local_node->GetPortByPort(k);

        name = new char[TOPGEN_MAX_STRING_LENGTH];
        snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", local_node_port->net_port/*local_node_port->decimal*/);
	    CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, local_node_port->net_port, "");
	    delete [] name;
	    _LoadIA(new_ia);
      }
    }
    // Channels
    group_channel_list = m_pGroupsArray[i]->GetGroupChannels();
    std::list<CDFlyChannel*>::iterator it = group_channel_list->begin();
    while (it != group_channel_list->end()) {
      //CDFlyChannel * ch = (*it);
      //_LoadChannel(ch);
      _LoadChannel((*it)->Create());
      ++it;
    }
  }  
}

void CDFly::LoadRoutes() {
  // TODO - Implement LoadRoutes()
}

int CDFly::GetGroupFromNetPort(int net_port){
  return (net_port / (m_iNodesPerGroup * m_iNetPortsPerNode));
}

int CDFly::GetGroupFromNode(int node_id){
  return (node_id / m_iNodesPerGroup);
}

int CDFly::GetNodeInGroupFromNetPort(int net_port){
  return (net_port % (m_iNodesPerGroup * m_iNetPortsPerNode)) / m_iNetPortsPerNode;
}

int CDFly::GetNodeInGroupFromNode(int node_id){
  return (node_id % m_iNodesPerGroup);
}

int CDFly::GetOutputport(int node_id, int dest){
  return m_pRoutingAlgorithm->DeterministicRouting(node_id,dest);
}

// --------------------------
// CDFlyRoutingAlgorithm class implementation
// --------------------------  

CDFlyRoutingAlgorithm::CDFlyRoutingAlgorithm(rtAlgorithmType rt_alg, CDFly * network){
  m_iRtAlgorithmType = rt_alg;
  m_pNetwork = network;
  m_iSwitchesPerGroup = m_pNetwork->GetSwitchesPerGroup();
  m_iGlobalLinksPerNode = m_pNetwork->GetGlobalLinksPerSwitch();
  m_iNetPortsPerNode = m_pNetwork->GetNodesPerSwitch();
}

CDFlyRoutingAlgorithm::~CDFlyRoutingAlgorithm(){

}

int CDFlyRoutingAlgorithm::DeterministicRouting(int local_node_id, int dest_port_id){

  int oPort = -1;
  int g_src = m_pNetwork->GetGroupFromNode(local_node_id);
  int g_dest = m_pNetwork->GetGroupFromNetPort(dest_port_id);
  int r_src = m_pNetwork->GetNodeInGroupFromNode(local_node_id);
  int r_dest = m_pNetwork->GetNodeInGroupFromNetPort(dest_port_id);
 
  if (g_dest != g_src) {
    if (g_dest > g_src) {
      g_dest -= 1;
    }
    int r_inter = g_dest / m_iGlobalLinksPerNode;
    if (r_src == r_inter) {
      oPort = (g_dest % m_iGlobalLinksPerNode) + m_iNetPortsPerNode;
    } else {
      oPort = r_inter + m_iGlobalLinksPerNode + m_iNetPortsPerNode;
 
      if (r_src < r_inter) {
	oPort -= 1;
      }
    }
 
  } else {
    if (r_dest > r_src) {
      oPort = r_dest - 1 + (m_iNetPortsPerNode + m_iGlobalLinksPerNode);
    } else if (r_dest < r_src) {
      oPort = r_dest + (m_iNetPortsPerNode + m_iGlobalLinksPerNode);
    } else {
      oPort = dest_port_id % m_iNetPortsPerNode;
    }
  }
  return oPort;
}

} // namespace topology
