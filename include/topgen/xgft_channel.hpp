/*
 *  XGFTChannel.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_XGFT_CHANNEL_HPP
#define TOPGEN_XGFT_CHANNEL_HPP

#include <string.h>
#include <topgen/xgft_node.hpp>
#include <topgen/node/channel_type_node.hpp>
#include <topgen/exception.hpp>

namespace topgen {

    class CXGFTChannel {
    public:
        CXGFTChannel(int id, channelType type, CXGFTNode * down_node, int down_port, CXGFTNode * up_node, int up_port);
        CXGFTChannel(int id, channelType type, CXGFTNode * node, int port, int ia);
        virtual ~CXGFTChannel();

        CChannelType_Node * Create();

        int m_iUpPort;
        int m_iDownPort;

        CXGFTNode * m_pUpNode;
        CXGFTNode * m_pDownNode;

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

#endif /* TOPGEN_XGFT_CHANNEL_HPP */
