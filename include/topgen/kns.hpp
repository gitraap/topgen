/*
 *  KNS.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_KNS_HPP
#define TOPGEN_KNS_HPP

#include <list>
#include <math.h>
#include <topgen/ia.hpp>
#include <topgen/router.hpp>
#include <topgen/topology.hpp>
#include <topgen/torusnd.hpp>
#include <topgen/min.hpp>
#include <topgen/kns_channel.hpp>
#include <topgen/exception.hpp>

namespace topgen {
    enum KNSIndirectEnum {
        knsmin, knsrouter
    };
    typedef enum KNSIndirectEnum knsIndirectType;
    
    struct stMIN {
        int * position; //common coordinates different from the dimension
        std::list<CElementType_Node*> vrouters; // virtual routers
        std::list<CElementType_Node*> vswitches; // virtual switches
        std::list<CElementType_Node*> tswitches; // template switches
    };

    class CKNS : public topology {
    public:
        CKNS(int dimensions, int * karies, knsIndirectType type, int * radixes, bool verbose);
        virtual ~CKNS();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildInterconnection();
        virtual void BuildChannels();
        virtual void BuildRoutes();

        virtual int GetNumberOfSwitches();
        virtual int GetNumberOfDestinations();
        virtual int GetNodes();

        virtual void LoadNetwork();
        virtual void LoadRoutes();

        virtual int GetPortToNIC();
        virtual int GetNICPort();
        virtual int GetPortToDimension(const int dimension);
        virtual int GetStagesDimension(const int d);
        virtual CElementType_Node * GetVirtRouter(int * coordinates);
        virtual struct stMIN * GetRefMIN(int dimension, CElementType_Node * vr);
        virtual void EmptyMIN(struct stMIN * min);
        virtual bool CheckCalculations();

    private:

        /* Templates for topologies */
        CTorusND * m_pDirect; // one direct
        CMIN ** m_pIndirect; // one indirect per dimension
        CRouter ** m_pIndirectRouter; // one indirect per dimension

        /* Virtual refers to hybrid topology */
        std::list<CElementType_Node*> * m_pVirtRouters; // Elements of direct
        std::list<CElementType_Node*> * m_pVirtSwitches; // Elements of indirect

        std::list<struct stMIN*> ** m_pMINs;

        knsIndirectType tIndirectType;
        int m_iNodes;
        int m_iIdentifiers;
        int m_iDestinations;
        nodeType m_iNodeType;
        int m_iPortToNIC;
        int m_iNICPort;
        int * m_pPortToDimension;

        std::list<CKNSChannel*> * m_pChannelList;
    };

} // namespace topology

#endif /* TOPGEN_KNS_HPP */
