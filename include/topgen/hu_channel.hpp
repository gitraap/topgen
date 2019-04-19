/*
 *  HUChannel.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_HU_CHANNEL_HPP
#define TOPGEN_HU_CHANNEL_HPP

#include <string.h>
#include <topgen/hu_node.hpp>
#include <topgen/exception.hpp>
#include <topgen/node/channel_type_node.hpp>

namespace topgen {

    class CHUChannel {
    public:
        CHUChannel(int id, channelType type, CHUNode * down_node, int down_port, CHUNode * up_node, int up_port);
        CHUChannel(int id, channelType type, CHUNode * node, int port, int ia);
        virtual ~CHUChannel();

        CChannelType_Node * Create();

        int m_iUpPort;
        int m_iDownPort;

        CHUNode * m_pUpNode;
        CHUNode * m_pDownNode;

        int m_iID;
        channelType m_eType;
        int m_iIA;

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

#endif /* TOPGEN_HU_CHANNEL_HPP */
