/*
 * RLFTChannel.h
 *
 *  Created on: 21/05/2013
 *      Author: jesus
 */

#ifndef TOPGEN_RLFT_CHANNEL_HPP
#define TOPGEN_RLFT_CHANNEL_HPP

#include <string.h>
#include <topgen/rlft_node.hpp>
#include <topgen/exception.hpp>
#include <topgen/node/channel_type_node.hpp>

namespace topgen {

    class CRLFTChannel {
    public:
        CRLFTChannel(int id, channelType type, CRLFTNode * down_node, rlftNodeStructType * down_port, CRLFTNode * up_node, rlftNodeStructType * up_port);
        CRLFTChannel(int id, channelType type, CRLFTNode * up_node, rlftNodeStructType * up_port);
        CRLFTChannel(int id, channelType type, CRLFTNode * node, int port);
        virtual ~CRLFTChannel();

        CChannelType_Node * Create();

        // raw attributes
        rlftNodeStructType * m_pUpPort;
        rlftNodeStructType * m_pDownPort;

        CRLFTNode * m_pUpNode;
        CRLFTNode * m_pDownNode;

        int m_iID;
        channelType m_eType;
        int m_iPort;

        // strings
        char m_sType[TOPGEN_MAX_STRING_LENGTH];
        char m_sName[TOPGEN_MAX_STRING_LENGTH];
        char m_sIdentifier[TOPGEN_MAX_STRING_LENGTH];
        char m_sUpElement[TOPGEN_MAX_STRING_LENGTH];
        char m_sUpPort[TOPGEN_MAX_STRING_LENGTH];
        char m_sDownElement[TOPGEN_MAX_STRING_LENGTH];
        char m_sDownPort[TOPGEN_MAX_STRING_LENGTH];
    };

} /* namespace topology */
#endif /* TOPGEN_RLFT_CHANNEL_HPP */
