/*
 *  DIRChannel.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_DIR_CHANNEL_HPP
#define TOPGEN_DIR_CHANNEL_HPP

#include <string.h>
#include <topgen/dir_node.hpp>
#include <topgen/node/channel_type_node.hpp>

namespace topgen {

class CChannelType_Node;

class CDIRChannel
{
public:
	CDIRChannel(channelType type, int id, CDIRNode * downNode, int downPort, CDIRNode * upNode, int upPort);
	virtual ~CDIRChannel();

	void Print(FILE * file);
	CChannelType_Node * Create();
	void PrintVRMLChannel(FILE * file);

	// raw values
	channelType m_eType;
	int m_iID;
	CDIRNode * m_pUpNode;
	CDIRNode * m_pDownNode;
	int m_iUpPort;
	int m_iDownPort;

	// strings
	char m_sType[TOPGEN_MAX_STRING_LENGTH];
	char m_sName[TOPGEN_MAX_STRING_LENGTH];
	char m_sIdentifier[TOPGEN_MAX_STRING_LENGTH];
	char m_sUpElement[TOPGEN_MAX_STRING_LENGTH];
	char m_sUpPort[TOPGEN_MAX_STRING_LENGTH];
	char m_sDownElement[TOPGEN_MAX_STRING_LENGTH];
	char m_sDownPort[TOPGEN_MAX_STRING_LENGTH];
};

} // namespace topology

#endif /* TOPGEN_DIR_CHANNEL_HPP */
