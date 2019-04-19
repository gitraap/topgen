/*
 *  DFlyChannel.h
 *
 *  Created by Juan Villar and Jesus Escudero-Sahuquillo on 01/11/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#ifndef DFLYCHANNEL_H_
#define DFLYCHANNEL_H_

#include <string.h>
#include <topgen/dfly_node.hpp>
#include <topgen/exception.hpp>
#include <topgen/node/channel_type_node.hpp>

namespace topgen {

    class CDFlyChannel {
    public:
        CDFlyChannel(int id, channelType type, CDFlyNode * down_node, dflyNodeStruct_t * down_port, CDFlyNode * up_node, dflyNodeStruct_t * up_port);
        CDFlyChannel(int id, channelType type, CDFlyNode * up_node, dflyNodeStruct_t * up_port);
        virtual ~CDFlyChannel();

        void Print();
        CChannelType_Node * Create();

	// raw attributes
	dflyNodeStruct_t * m_pUpPort;
	dflyNodeStruct_t * m_pDownPort;

        CDFlyNode * m_pUpNode;
        CDFlyNode * m_pDownNode;

        int m_iID;
        channelType m_eType;
        static const size_t MAX_STRING_LENGTH = 16;

        // strings
        char m_sType[MAX_STRING_LENGTH];
        char m_sName[MAX_STRING_LENGTH];
        char m_sIdentifier[MAX_STRING_LENGTH];
        char m_sUpElement[MAX_STRING_LENGTH];
        char m_sUpPort[MAX_STRING_LENGTH];
        char m_sDownElement[MAX_STRING_LENGTH];
        char m_sDownPort[MAX_STRING_LENGTH];
    };

} // namespace topology

#endif /* DFLYCHANNEL_H_ */
