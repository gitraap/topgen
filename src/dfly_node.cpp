/*
 *  DFlyNode.cpp
 *
 *  Created by Juan Villar and Jesus Escudero-Sahuquillo on 01/11/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include "topgen/dfly_node.hpp"

namespace topgen {

CDFlyNode::CDFlyNode(int identifier, int a, int h, int p)
{
	m_iIdentifier = identifier;
	m_iGlobalLinks = h;
	m_iLocalLinks = a;
	m_iNetPortLinks = p;
	m_iPorts = m_iGlobalLinks + (m_iLocalLinks - 1) + m_iNetPortLinks;

	m_sName = new char[MAX_STRING_LENGTH];
	snprintf(m_sName, MAX_STRING_LENGTH, "s%d", m_iIdentifier);

	m_pNodePortsArray = new dflyNodeStruct_t * [m_iPorts];

	m_iID=g_iCounter++;
	m_pGroup = NULL;
}

CDFlyNode::~CDFlyNode()
{
	delete [] m_sName;
	
	for (int i = 0; i < m_iPorts; i++){
          delete m_pNodePortsArray[i];
        }
	delete [] m_pNodePortsArray;
}

int CDFlyNode::g_iCounter=0; /* Initialize static counter */

void CDFlyNode::SetNetPort(int port, int net_port){
    m_pNodePortsArray[port]->net_port = net_port;
}

int CDFlyNode::GetNetPort(int port) {
    return m_pNodePortsArray[port]->net_port;
}

void CDFlyNode::SetGroup(CDFlyGroup * group){
  m_pGroup = group;
}

int CDFlyNode::GetID()
{
    return m_iID;
}

int CDFlyNode::GetPorts()
{
    return m_iPorts;
}

int CDFlyNode::GetIdentifier()
{
    return m_iIdentifier;
}

CDFlyGroup * CDFlyNode::GetGroup(){
    return m_pGroup;
}

char * CDFlyNode::GetName()
{
    return m_sName;
}

void CDFlyNode::Init()
{
  for (int i = 0; i < m_iPorts; i++){
    
    m_pNodePortsArray[i] = new dflyNodeStruct_t;
    m_pNodePortsArray[i]->port = i;
    m_pNodePortsArray[i]->decimal = i;
    m_pNodePortsArray[i]->remote_node = NULL;
    m_pNodePortsArray[i]->remote_port = NULL;
    m_pNodePortsArray[i]->net_port = -1;

    if(i < m_iNetPortLinks){
      SetNetPort(i, (m_iIdentifier * m_iNetPortLinks) + i);
    }
  }
}

dflyNodeStruct_t * CDFlyNode::GetPortByPort(int port){
  return m_pNodePortsArray[port];
}

void CDFlyNode::Print(FILE * file, const char * type)
{
  // Nothing - Preserved for backwards compatibility
}

void CDFlyNode::Dump()
{
/*	g_pLog->AddF("m_iIdentifier    = %d\n", m_iIdentifier);
	g_pLog->AddF("m_iPorts         = %d\n", m_iPorts);
	g_pLog->AddF("m_iGlobalLinks   = %d\n", m_iGlobalLinks);
	g_pLog->AddF("m_iLocalLinks    = %d\n", m_iLocalLinks);
	g_pLog->AddF("m_iNetPortLinks  = %d\n", m_iNetPortLinks);
	g_pLog->AddF("m_sName          = %s\n", m_sName);
	g_pLog->AddF("m_iID            = %d\n", m_iID);
	g_pLog->AddL("m_pNetPortsArray = ");
	for(int i=0; i<m_iPorts; i++)
	  g_pLog->AddF("%d|", m_pNodePortsArray[i]);
*/
}

void CDFlyNode::ResetCounter()
{
    g_iCounter = 0; // required when GTest launches various test in sequence
}

}  // namespace topology
