/*
 * tester.c
 *
 *  Created on: Nov 14, 2011
 *      Author: Juan A. Villar
 */

#include <stdlib.h>
#include <gtest/gtest.h>
#include <topgen.hpp>

class CParameters {
public:
    /* Topology Generator */
    topgen::topologyType m_eTopology;
    unsigned int m_iEndNodes = 0;
    unsigned int m_iRadix = 0;
    int * m_pRadixes = NULL;
    unsigned int m_iInternalPorts = 0;
    topgen::connectionType m_eConnection;
    topgen::rtAlgorithmType m_eRoutingAlgorithm;
    topgen::nodeType m_eEmbedded;
    topgen::matchingType m_eMatching;
    unsigned int m_iKarity1 = 0;
    unsigned int m_iKarity2 = 0;
    unsigned int m_iKarity3 = 0;
    unsigned int m_iDimensions = 0;
    int * m_pKaries = NULL;
    int m_iBits = 0;
    char * m_sPatternFile = NULL;
    int m_iHeight = 0;
    int * m_pChildren = NULL;
    int * m_pParents = NULL;
    topgen::KNSIndirectEnum m_tIndirectTopo;
    bool m_bVerbose = false;
    int m_iTopologyParams = -1;
    char ** m_pTopologyParams = NULL;
    int m_iRoutingParams = -1;
    char ** m_pRoutingParams = NULL;
    int m_iA = -1; //Dragonfly
    int m_iH = -1;
    int m_iP = -1;

    CParameters() {
        /* DO NOT MODIFY THIS VALUES */
        m_iInternalPorts = 0;
        m_eConnection = topgen::butterfly;
        m_eRoutingAlgorithm = topgen::destro;
        m_eEmbedded = topgen::switchSTD;
        m_eMatching = topgen::Alpha;
        m_pKaries = NULL;
        m_pRadixes = NULL;
        m_sPatternFile = NULL;
    };

    ~CParameters() {
    };
};

topgen::topology * DefineNewTopology(CParameters * params) {
    try {
        /* Initialization of the library */
        topgen::topology * m_pTopology;
        switch (params->m_eTopology) {
            case topgen::custom1:
                m_pTopology = new topgen::CCustom1();
                break;

            case topgen::custom2:
                m_pTopology = new topgen::CCustom2();
                break;

            case topgen::fully:
                m_pTopology = new topgen::CFully(params->m_iEndNodes);
                break;

            case topgen::router:
                m_pTopology = new topgen::CRouter(params->m_iRadix);
                break;

            case topgen::cube:
                m_pTopology = new topgen::CCube(params->m_iDimensions);
                break;

            case topgen::mesh:
                m_pTopology = new topgen::CMesh(params->m_iDimensions,
                    params->m_iKarity1,
                    params->m_iKarity2,
                    params->m_iKarity3);
                break;

            case topgen::torus:
                m_pTopology = new topgen::CTorus(params->m_iDimensions,
                    params->m_iKarity1,
                    params->m_iKarity2,
                    params->m_iKarity3);
                ((topgen::CTorus*)m_pTopology)->Settings(params->m_eEmbedded);
                break;

            case topgen::torusND:
                m_pTopology = new topgen::CTorusND(params->m_iDimensions, params->m_pKaries);
                break;

            case topgen::min:
                m_pTopology = new topgen::CMIN(params->m_iEndNodes,
                    params->m_iRadix,
                    params->m_eConnection,
                    params->m_eRoutingAlgorithm);
                ((topgen::CMIN*)m_pTopology)->Settings(topgen::standard,
                    params->m_eEmbedded,
                    params->m_eMatching,
                    params->m_iInternalPorts,
                    params->m_sPatternFile);
                break;

            case topgen::kns:
                m_pTopology = new topgen::CKNS(params->m_iDimensions,
                    params->m_pKaries,
                    params->m_tIndirectTopo,
                    params->m_pRadixes,
                    params->m_bVerbose);
                break;

            case topgen::xgft:
                m_pTopology = new topgen::CXGFT(params->m_iRadix,
                    params->m_iHeight,
                    params->m_pChildren,
                    params->m_pParents,
                    false);
                break;

            case topgen::customhu:
                m_pTopology = new topgen::CCustomHU(
                    params->m_iTopologyParams,
                    params->m_pTopologyParams,
                    params->m_iRoutingParams,
                    params->m_pRoutingParams);
                break;

            case topgen::rlft:
                m_pTopology = new topgen::CRLFT(params->m_iEndNodes,
                                                params->m_iRadix,
                                                params->m_eConnection,
                                                params->m_eRoutingAlgorithm,
                                                true);
                break;  

            case topgen::dfly:
                m_pTopology = new topgen::CDFly(params->m_iA, 
                                                params->m_iH,
                                                params->m_iP, 
                                                params->m_eConnection,
                                                params->m_eRoutingAlgorithm);
                break;

            default:
                throw new topgen::CException("Topology not supported");
        }

        m_pTopology->RunAll();

        return m_pTopology;
    } catch (topgen::CException * e) {
        printf("Message from CException: `%s'\n", e->Report());
        fflush(stdout);

        return NULL;
    }
}

TEST(fast, router_48) {

    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::router;
    g_pParameter->m_iRadix = 48;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(48, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(1, switches);

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, mesh_2x2)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::mesh;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_iKarity1 = 2;
    g_pParameter->m_iKarity2 = 2;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(4, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(4, switches);
    int num_ports = m_pTopology->GetNumberOfPorts(0);
    EXPECT_EQ(3, num_ports);

//    m_pTopology->printTopology();

//    /* Print the routing */
//    for (unsigned int i=0; i<switches; ++i) {
//        for (unsigned int j = 0; j < endnodes; ++j) {
//            int outputport = m_pTopology->GetOutputport(i, j);
//            printf("Routing sw_id %d dest %d outport %u \n",i,j,outputport);
//        }
//    }

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, mesh_4x4x4)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::mesh;
    g_pParameter->m_iDimensions = 3;
    g_pParameter->m_iKarity1 = 4;
    g_pParameter->m_iKarity2 = 4;
    g_pParameter->m_iKarity3 = 4;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(64, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(64, switches);

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, mesh_8x8x8)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::mesh;
    g_pParameter->m_iDimensions = 3;
    g_pParameter->m_iKarity1 = 8;
    g_pParameter->m_iKarity2 = 8;
    g_pParameter->m_iKarity3 = 8;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(512, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(512, switches);

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, torus_2x2)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::torus;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_iKarity1 = 2;
    g_pParameter->m_iKarity2 = 2;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(4, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(4, switches);
    int num_ports = m_pTopology->GetNumberOfPorts(0);
    EXPECT_EQ(5, num_ports);

    m_pTopology->printTopology();

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, torus_2x2x2)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::torus;
    g_pParameter->m_iDimensions = 3;
    g_pParameter->m_iKarity1 = 2;
    g_pParameter->m_iKarity2 = 2;
    g_pParameter->m_iKarity3 = 2;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(8, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(8, switches);
    int num_ports = m_pTopology->GetNumberOfPorts(0);
    EXPECT_EQ(7, num_ports);

    m_pTopology->printTopology();

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, torus_8x8x8)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::torus;
    g_pParameter->m_iDimensions = 3;
    g_pParameter->m_iKarity1 = 8;
    g_pParameter->m_iKarity2 = 8;
    g_pParameter->m_iKarity3 = 8;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(512, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(512, switches);

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, torusND_4x4)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::torusND;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_pKaries = new int[2];
    g_pParameter->m_pKaries[0] = 4;
    g_pParameter->m_pKaries[1] = 4;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    topgen::CTorusND * torito = (topgen::CTorusND*) m_pTopology;

    EXPECT_EQ(2, torito->GetDimensions());
    EXPECT_EQ(4, torito->GetKary(1));
    EXPECT_EQ(4, torito->GetKary(2));

    EXPECT_EQ(16, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(16, m_pTopology->NumberOfSwitches());

    EXPECT_EQ(0, m_pTopology->GetInputAdapterIdentifier(0));
    EXPECT_EQ(7, m_pTopology->GetInputAdapterIdentifier(7));
    EXPECT_EQ(15, m_pTopology->GetInputAdapterIdentifier(15));

    EXPECT_EQ(0, m_pTopology->GetElementIdentifier(0));
    EXPECT_EQ(7, m_pTopology->GetElementIdentifier(7));
    EXPECT_EQ(15, m_pTopology->GetElementIdentifier(15));

    delete m_pTopology;
    delete [] g_pParameter->m_pKaries;
    delete g_pParameter;
}

TEST(fast, torusND_4x8)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::torusND;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_pKaries = new int[2];
    g_pParameter->m_pKaries[0] = 4;
    g_pParameter->m_pKaries[1] = 8;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    topgen::CTorusND * torito = (topgen::CTorusND*) m_pTopology;

    EXPECT_EQ(2, torito->GetDimensions());
    EXPECT_EQ(4, torito->GetKary(1));
    EXPECT_EQ(8, torito->GetKary(2));

    EXPECT_EQ(32, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(32, m_pTopology->NumberOfSwitches());

    EXPECT_EQ(0, m_pTopology->GetInputAdapterIdentifier(0));
    EXPECT_EQ(15, m_pTopology->GetInputAdapterIdentifier(15));
    EXPECT_EQ(31, m_pTopology->GetInputAdapterIdentifier(31));

    EXPECT_EQ(0, m_pTopology->GetElementIdentifier(0));
    EXPECT_EQ(15, m_pTopology->GetElementIdentifier(15));
    EXPECT_EQ(31, m_pTopology->GetElementIdentifier(31));

    delete m_pTopology;
    delete [] g_pParameter->m_pKaries;
    delete g_pParameter;
}

TEST(fast, min_4_2n)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::min;
    g_pParameter->m_iEndNodes = 4; // assuming 3 stages
    g_pParameter->m_iRadix = 4;
    g_pParameter->m_eConnection = topgen::butterfly;
    g_pParameter->m_eRoutingAlgorithm = topgen::destro;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);

    EXPECT_EQ(4, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(4, m_pTopology->NumberOfSwitches());

    m_pTopology->printTopology();

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, min_64_3n)
{
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::min;
    g_pParameter->m_iEndNodes = 64; // assuming 3 stages
    g_pParameter->m_iRadix = 8;
    g_pParameter->m_eConnection = topgen::butterfly;
    g_pParameter->m_eRoutingAlgorithm = topgen::destro;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);

    EXPECT_EQ(64, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(48, m_pTopology->NumberOfSwitches());
    //((topgen::CMIN)m_pTopology)->

    delete m_pTopology;
    delete g_pParameter;
}

TEST(slow, min_24k_3n) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::min;
    g_pParameter->m_iEndNodes = 32768; // assuming 3 stages
    g_pParameter->m_iRadix = 64;
    g_pParameter->m_eConnection = topgen::butterfly;
    g_pParameter->m_eRoutingAlgorithm = topgen::destro;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(32768, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(3072, switches);

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, kns_2d_4x4_ft_4) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::kns;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_pKaries = new int[2];
    g_pParameter->m_pKaries[0] = 4;
    g_pParameter->m_pKaries[1] = 4;
    g_pParameter->m_tIndirectTopo = topgen::knsmin;
    g_pParameter->m_pRadixes = new int[2];
    g_pParameter->m_pRadixes[0] = 4;
    g_pParameter->m_pRadixes[1] = 4;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    topgen::CKNS * mykns = (topgen::CKNS*) m_pTopology;

    // configuration
    EXPECT_EQ(16, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(48, m_pTopology->NumberOfSwitches());

    // IAs
    EXPECT_EQ(0, m_pTopology->GetInputAdapterIdentifier(0));
    EXPECT_EQ(3, m_pTopology->GetInputAdapterIdentifier(3));
    EXPECT_EQ(12, m_pTopology->GetInputAdapterIdentifier(12));
    EXPECT_EQ(15, m_pTopology->GetInputAdapterIdentifier(15));

    // routers
    EXPECT_EQ(0, m_pTopology->GetElementIdentifier(0));
    EXPECT_EQ(3, m_pTopology->GetElementIdentifier(3));
    EXPECT_EQ(12, m_pTopology->GetElementIdentifier(12));
    EXPECT_EQ(15, m_pTopology->GetElementIdentifier(15));

    // routers connect to IA in port (number of dimensions)
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(3, 2, ia_id, portid));
    EXPECT_EQ(3, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(7, 2, ia_id, portid));
    EXPECT_EQ(7, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(11, 2, ia_id, portid));
    EXPECT_EQ(11, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(15, 2, ia_id, portid));
    EXPECT_EQ(15, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    // dimension horizontal
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(4, 0, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(5, 0, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(6, 0, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(7, 0, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(1, portid[0]);

    // Dimension vertical
    EXPECT_EQ(true, m_pTopology->IsElementToElement(3, 1, switchid, portid));
    EXPECT_EQ(44, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(7, 1, switchid, portid));
    EXPECT_EQ(44, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(11, 1, switchid, portid));
    EXPECT_EQ(45, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(15, 1, switchid, portid));
    EXPECT_EQ(45, switchid[0]);
    EXPECT_EQ(1, portid[0]);

    // MIN d=0 p={X,1}
    EXPECT_EQ(true, m_pTopology->IsElementToElement(20, 2, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(20, 3, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(21, 2, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(21, 3, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(1, portid[0]);

    // Check the number of elements based on the paper of KNS
    EXPECT_EQ(true, mykns->CheckCalculations());

    delete m_pTopology;
    delete [] g_pParameter->m_pKaries;
    delete [] g_pParameter->m_pRadixes;
    delete g_pParameter;
}

TEST(fast, kns_2d_4x4_rt_4) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::kns;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_pKaries = new int[2];
    g_pParameter->m_pKaries[0] = 4;
    g_pParameter->m_pKaries[1] = 4;
    g_pParameter->m_tIndirectTopo = topgen::knsrouter;
    g_pParameter->m_pRadixes = new int[2];
    g_pParameter->m_pRadixes[0] = 4;
    g_pParameter->m_pRadixes[1] = 4;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    topgen::CKNS * mykns = (topgen::CKNS*) m_pTopology;

    // configuration
    EXPECT_EQ(16, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(24, m_pTopology->NumberOfSwitches());

    // IAs
    for (int ia = 0; ia < 16; ia++)
        EXPECT_EQ(ia, m_pTopology->GetInputAdapterIdentifier(ia));

    // routers
    for (int rt = 0; rt < 24; rt++)
        EXPECT_EQ(rt, m_pTopology->GetElementIdentifier(rt));

    //int m_pChildren[2] = {18, 36};
    //int m_pParents[2] = {18, 18};

    // routers connect to IA in port (number of dimensions)
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(3, 2, ia_id, portid));
    EXPECT_EQ(3, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(7, 2, ia_id, portid));
    EXPECT_EQ(7, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(11, 2, ia_id, portid));
    EXPECT_EQ(11, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(15, 2, ia_id, portid));
    EXPECT_EQ(15, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    // dimension horizontal
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(0, 0, switchid, portid));
    EXPECT_EQ(16, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(1, 0, switchid, portid));
    EXPECT_EQ(16, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(2, 0, switchid, portid));
    EXPECT_EQ(16, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(3, 0, switchid, portid));
    EXPECT_EQ(16, switchid[0]);
    EXPECT_EQ(3, portid[0]);
    //
    EXPECT_EQ(true, m_pTopology->IsElementToElement(4, 0, switchid, portid));
    EXPECT_EQ(17, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(5, 0, switchid, portid));
    EXPECT_EQ(17, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(6, 0, switchid, portid));
    EXPECT_EQ(17, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(7, 0, switchid, portid));
    EXPECT_EQ(17, switchid[0]);
    EXPECT_EQ(3, portid[0]);
    //
    EXPECT_EQ(true, m_pTopology->IsElementToElement(8, 0, switchid, portid));
    EXPECT_EQ(18, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(9, 0, switchid, portid));
    EXPECT_EQ(18, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(10, 0, switchid, portid));
    EXPECT_EQ(18, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(11, 0, switchid, portid));
    EXPECT_EQ(18, switchid[0]);
    EXPECT_EQ(3, portid[0]);
    //
    EXPECT_EQ(true, m_pTopology->IsElementToElement(12, 0, switchid, portid));
    EXPECT_EQ(19, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(13, 0, switchid, portid));
    EXPECT_EQ(19, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(14, 0, switchid, portid));
    EXPECT_EQ(19, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(15, 0, switchid, portid));
    EXPECT_EQ(19, switchid[0]);
    EXPECT_EQ(3, portid[0]);

    // Dimension vertical
    EXPECT_EQ(true, m_pTopology->IsElementToElement(0, 1, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(4, 1, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(8, 1, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(12, 1, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(3, portid[0]);
    //
    EXPECT_EQ(true, m_pTopology->IsElementToElement(1, 1, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(5, 1, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(9, 1, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(13, 1, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(3, portid[0]);
    //
    EXPECT_EQ(true, m_pTopology->IsElementToElement(2, 1, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(6, 1, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(10, 1, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(14, 1, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(3, portid[0]);
    //
    EXPECT_EQ(true, m_pTopology->IsElementToElement(3, 1, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(7, 1, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(11, 1, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(15, 1, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(3, portid[0]);

    // Check the number of elements based on the paper of KNS
    EXPECT_EQ(true, mykns->CheckCalculations());

    delete m_pTopology;
    delete [] g_pParameter->m_pKaries;
    delete [] g_pParameter->m_pRadixes;
    delete g_pParameter;
}

TEST(fast, xgft_2h_3x3_1x2_5) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::xgft;
    g_pParameter->m_iHeight = 2;
    g_pParameter->m_pChildren = new int[2];
    g_pParameter->m_pChildren[0] = 3;
    g_pParameter->m_pChildren[1] = 3;
    g_pParameter->m_pParents = new int[2];
    g_pParameter->m_pParents[0] = 1;
    g_pParameter->m_pParents[1] = 2;
    g_pParameter->m_iRadix = 5;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    //topgen::CXGFT * myxgft = (topgen::CXGFT*) m_pTopology;

    // configuration
    EXPECT_EQ(9, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(5, m_pTopology->NumberOfSwitches());

    // routers
    EXPECT_EQ(0, m_pTopology->GetInputAdapterIdentifier(0));
    EXPECT_EQ(4, m_pTopology->GetInputAdapterIdentifier(4));
    EXPECT_EQ(6, m_pTopology->GetInputAdapterIdentifier(6));
    EXPECT_EQ(8, m_pTopology->GetInputAdapterIdentifier(8));

    // switches
    EXPECT_EQ(0, m_pTopology->GetElementIdentifier(0));
    EXPECT_EQ(1, m_pTopology->GetElementIdentifier(1));
    EXPECT_EQ(2, m_pTopology->GetElementIdentifier(2));
    EXPECT_EQ(3, m_pTopology->GetElementIdentifier(3));
    EXPECT_EQ(4, m_pTopology->GetElementIdentifier(4));

    // router (1,2)
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(2, 0, ia_id, portid));
    EXPECT_EQ(6, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(2, 1, ia_id, portid));
    EXPECT_EQ(7, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(2, 2, ia_id, portid));
    EXPECT_EQ(8, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    //
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(2, 3, switchid, portid));
    EXPECT_EQ(3, switchid[0]);
    EXPECT_EQ(2, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(2, 4, switchid, portid));
    EXPECT_EQ(4, switchid[0]);
    EXPECT_EQ(2, portid[0]);

    // Check the number of elements based on the paper of KNS
    //EXPECT_EQ(true, myxgft->CheckCalculations());

    delete m_pTopology;
    delete [] g_pParameter->m_pChildren;
    delete [] g_pParameter->m_pParents;
    delete g_pParameter;
}



TEST(fast, xgft_2h_26x48_1x22_48) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::xgft;
    g_pParameter->m_iHeight = 2;
    g_pParameter->m_pChildren = new int[2];
    g_pParameter->m_pChildren[0] = 26;
    g_pParameter->m_pChildren[1] = 48;
    g_pParameter->m_pParents = new int[2];
    g_pParameter->m_pParents[0] = 1;
    g_pParameter->m_pParents[1] = 22;
    g_pParameter->m_iRadix = 48;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    //topgen::CXGFT * myxgft = (topgen::CXGFT*) m_pTopology;

    // configuration
    EXPECT_EQ(1248, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(70, m_pTopology->NumberOfSwitches());

    // IAs
    for (int ia = 0; ia < 1248; ia++) {
        EXPECT_EQ(ia, m_pTopology->GetInputAdapterIdentifier(ia));
    }

    // switches
    for (int sw = 0; sw < 70; sw++) {

        EXPECT_EQ(sw, m_pTopology->GetElementIdentifier(sw));
    }

    // routers
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(0, 0, ia_id, portid));
    EXPECT_EQ(0, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(1, 0, ia_id, portid));
    EXPECT_EQ(26, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(47, 25, ia_id, portid));
    EXPECT_EQ(1247, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    //
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(0, 47, switchid, portid));
    EXPECT_EQ(69, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(47, 26, switchid, portid));
    EXPECT_EQ(48, switchid[0]);
    EXPECT_EQ(47, portid[0]);

    // Check the number of elements based on the paper of KNS
    //EXPECT_EQ(true, myxgft->CheckCalculations());

    delete m_pTopology;
    delete [] g_pParameter->m_pChildren;
    delete [] g_pParameter->m_pParents;
    delete g_pParameter;
}

TEST(fast, rlft_128n_2l){
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::rlft;
    g_pParameter->m_iEndNodes = 128;
    g_pParameter->m_iRadix = 16;
    g_pParameter->m_bVerbose = false;
    g_pParameter->m_eConnection = topgen::butterfly;
    g_pParameter->m_eRoutingAlgorithm = topgen::destro;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(128, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(24, switches);
    int outputport = m_pTopology->GetOutputport(0, 1);
    EXPECT_EQ(1, outputport);

    /* Print the routing */
    for (int i=0; i<switches; ++i) {
        for (int j = 0; j < endnodes; ++j) {
            int outputport = m_pTopology->GetOutputport(i, j);
            //int outputport = ((CRLFT *)m_pTopology)->GetOutputport(i, j);
            printf("Routing sw_id %d dest %d outport %u \n",i,j,outputport);
        }
    }

    delete m_pTopology;
    delete g_pParameter;
}

TEST(fast, customhu_5x2_H) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::customhu;
    g_pParameter->m_iTopologyParams = 3;
    g_pParameter->m_pTopologyParams = new char*[3];
    for (int i = 0; i < 3; i++)
        g_pParameter->m_pTopologyParams[i] = new char[10];
    sprintf(g_pParameter->m_pTopologyParams[0], "hu-star");
    sprintf(g_pParameter->m_pTopologyParams[1], "5");
    sprintf(g_pParameter->m_pTopologyParams[2], "2");
    g_pParameter->m_iRoutingParams = 1;
    g_pParameter->m_pRoutingParams = new char*[1];
    g_pParameter->m_pRoutingParams[0] = new char[10];
    sprintf(g_pParameter->m_pRoutingParams[0], "H");

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    //topgen::CCustomHU * mycustomhu = (topgen::CCustomHU*) m_pTopology;

    // configuration
    EXPECT_EQ(21, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(6, m_pTopology->NumberOfSwitches());

    // IAs
    for (int ia = 0; ia < 21; ia++) {
        EXPECT_EQ(ia, m_pTopology->GetInputAdapterIdentifier(ia));
    }

    // switches
    for (int sw = 0; sw < 6; sw++) {
        EXPECT_EQ(sw, m_pTopology->GetElementIdentifier(sw));
    }

    // routers
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(0, 3, ia_id, portid));
    EXPECT_EQ(0, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(0, 7, ia_id, portid));
    EXPECT_EQ(4, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(5, 4, ia_id, portid));
    EXPECT_EQ(20, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    //
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(2, 0, switchid, portid));
    EXPECT_EQ(1, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(3, 1, switchid, portid));
    EXPECT_EQ(4, switchid[0]);
    EXPECT_EQ(0, portid[0]);

    delete m_pTopology;
    delete [] g_pParameter->m_pTopologyParams[0];
    delete [] g_pParameter->m_pTopologyParams[1];
    delete [] g_pParameter->m_pTopologyParams[2];
    delete [] g_pParameter->m_pTopologyParams;
    delete [] g_pParameter->m_pRoutingParams[0];
    delete [] g_pParameter->m_pRoutingParams;
    delete g_pParameter;
}

TEST(bgl, customhu_5x2_H) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::customhu;
    g_pParameter->m_iTopologyParams = 3;
    g_pParameter->m_pTopologyParams = new char*[3];
    for (int i = 0; i < 3; i++)
        g_pParameter->m_pTopologyParams[i] = new char[10];
    sprintf(g_pParameter->m_pTopologyParams[0], "hu-star");
    sprintf(g_pParameter->m_pTopologyParams[1], "5");
    sprintf(g_pParameter->m_pTopologyParams[2], "2");
    g_pParameter->m_iRoutingParams = 1;
    g_pParameter->m_pRoutingParams = new char*[1];
    g_pParameter->m_pRoutingParams[0] = new char[10];
    sprintf(g_pParameter->m_pRoutingParams[0], "H");

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    //topgen::CCustomHU * mycustomhu = (topgen::CCustomHU*) m_pTopology;

    // configuration
    EXPECT_EQ(21, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(6, m_pTopology->NumberOfSwitches());

    // IAs
    for (int ia = 0; ia < 21; ia++) {
        EXPECT_EQ(ia, m_pTopology->GetInputAdapterIdentifier(ia));
    }

    // switches
    for (int sw = 0; sw < 6; sw++) {
        EXPECT_EQ(sw, m_pTopology->GetElementIdentifier(sw));
    }

    // routers
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(0, 3, ia_id, portid));
    EXPECT_EQ(0, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(0, 7, ia_id, portid));
    EXPECT_EQ(4, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(5, 4, ia_id, portid));
    EXPECT_EQ(20, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    //
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(2, 0, switchid, portid));
    EXPECT_EQ(1, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(3, 1, switchid, portid));
    EXPECT_EQ(4, switchid[0]);
    EXPECT_EQ(0, portid[0]);

#ifndef TOPGEN_PBGL
    // BGL part
    std::string outfile("network.gv");
    m_pTopology->build_bgl();
#endif
    
    delete m_pTopology;
    delete [] g_pParameter->m_pTopologyParams[0];
    delete [] g_pParameter->m_pTopologyParams[1];
    delete [] g_pParameter->m_pTopologyParams[2];
    delete [] g_pParameter->m_pTopologyParams;
    delete [] g_pParameter->m_pRoutingParams[0];
    delete [] g_pParameter->m_pRoutingParams;
    delete g_pParameter;
}

TEST(bgl, dfly_a2_h3_p3){
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::dfly;
    g_pParameter->m_iEndNodes = 42;
    g_pParameter->m_iRadix = 14;
    g_pParameter->m_bVerbose = false;
    g_pParameter->m_iA = 2;
    g_pParameter->m_iH = 3;
    g_pParameter->m_iP = 3;
    g_pParameter->m_eConnection = topgen::dfly_canonic;
    g_pParameter->m_eRoutingAlgorithm = topgen::minimal;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(42, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(14, switches);
    int outputport = m_pTopology->GetOutputport(0, 1);
    EXPECT_EQ(1, outputport);

    /* Print the routing */
    //for (int i=0; i<switches; ++i) {
        //for (int j = 0; j < endnodes; ++j) {
            //int outputport = m_pTopology->GetOutputport(i, j);
            //int outputport = ((CRLFT *)m_pTopology)->GetOutputport(i, j);
            //printf("Routing sw_id %d dest %d outport %u \n",i,j,outputport);
        //}
    //}

#ifndef TOPGEN_PBGL    
    std::string dotfile("network.gv");
    m_pTopology->build_bgl();
    //m_pTopology->printTopology();
#endif
    
    delete m_pTopology;
    delete g_pParameter;
}

TEST(bgl, dfly_a4_h2_p2){
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::dfly;
    g_pParameter->m_iEndNodes = 41;
    g_pParameter->m_iRadix = 14;
    g_pParameter->m_bVerbose = false;
    g_pParameter->m_iA = 4;
    g_pParameter->m_iH = 2;
    g_pParameter->m_iP = 2;
    g_pParameter->m_eConnection = topgen::dfly_canonic;
    g_pParameter->m_eRoutingAlgorithm = topgen::minimal;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(72, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(36, switches);
    int outputport = m_pTopology->GetOutputport(0, 1);
    EXPECT_EQ(1, outputport);

    /* Print the routing */
    //for (int i=0; i<switches; ++i) {
        //for (int j = 0; j < endnodes; ++j) {
            //int outputport = m_pTopology->GetOutputport(i, j);
            //int outputport = ((CRLFT *)m_pTopology)->GetOutputport(i, j);
            //printf("Routing sw_id %d dest %d outport %u \n",i,j,outputport);
        //}
    //}

#ifndef TOPGEN_PBGL    
    m_pTopology->build_bgl();
    m_pTopology->run_dijkstra_shortest_paths(0,23);
    m_pTopology->run_bellman_ford_shortest_paths(0,23);
    m_pTopology->run_breadth_first_search(0);
    m_pTopology->run_depth_first_search(0);
    m_pTopology->run_kruskal_minimum_spanning_tree(0);
    m_pTopology->run_prim_minimum_spanning_tree(0);
    std::string dotfile("network.gv");
    m_pTopology->write_graphviz(dotfile);
    std::string mlfile("network.ml");
    m_pTopology->write_graphml(mlfile);
#else
    //std::string infile("network.gv");
    //topgen::run_pbgl_read_graphviz(infile);
    //topgen::run_pbgl_dijkstra_shortest_paths(0,NULL,0,23);//el 0,NULL para que compile
#endif
    
    delete m_pTopology;
    delete g_pParameter;
}

TEST(bgl, kns_2d_4x4_rt_4) {
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::kns;
    g_pParameter->m_iDimensions = 2;
    g_pParameter->m_pKaries = new int[2];
    g_pParameter->m_pKaries[0] = 4;
    g_pParameter->m_pKaries[1] = 4;
    g_pParameter->m_tIndirectTopo = topgen::knsrouter;
    g_pParameter->m_pRadixes = new int[2];
    g_pParameter->m_pRadixes[0] = 4;
    g_pParameter->m_pRadixes[1] = 4;
    g_pParameter->m_bVerbose = false;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    topgen::CKNS * mykns = (topgen::CKNS*) m_pTopology;

    // configuration
    EXPECT_EQ(16, m_pTopology->NumberOfDestinations());
    EXPECT_EQ(48, m_pTopology->NumberOfSwitches());

    // IAs
    EXPECT_EQ(0, m_pTopology->GetInputAdapterIdentifier(0));
    EXPECT_EQ(3, m_pTopology->GetInputAdapterIdentifier(3));
    EXPECT_EQ(12, m_pTopology->GetInputAdapterIdentifier(12));
    EXPECT_EQ(15, m_pTopology->GetInputAdapterIdentifier(15));

    // routers
    EXPECT_EQ(0, m_pTopology->GetElementIdentifier(0));
    EXPECT_EQ(3, m_pTopology->GetElementIdentifier(3));
    EXPECT_EQ(12, m_pTopology->GetElementIdentifier(12));
    EXPECT_EQ(15, m_pTopology->GetElementIdentifier(15));

    // routers connect to IA in port (number of dimensions)
    unsigned int ia_id[1];
    unsigned int portid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToIA(3, 2, ia_id, portid));
    EXPECT_EQ(3, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(7, 2, ia_id, portid));
    EXPECT_EQ(7, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(11, 2, ia_id, portid));
    EXPECT_EQ(11, ia_id[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToIA(15, 2, ia_id, portid));
    EXPECT_EQ(15, ia_id[0]);
    EXPECT_EQ(0, portid[0]);

    // dimension horizontal
    unsigned int switchid[1];
    EXPECT_EQ(true, m_pTopology->IsElementToElement(4, 0, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(5, 0, switchid, portid));
    EXPECT_EQ(20, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(6, 0, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(7, 0, switchid, portid));
    EXPECT_EQ(21, switchid[0]);
    EXPECT_EQ(1, portid[0]);

    // Dimension vertical
    EXPECT_EQ(true, m_pTopology->IsElementToElement(3, 1, switchid, portid));
    EXPECT_EQ(44, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(7, 1, switchid, portid));
    EXPECT_EQ(44, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(11, 1, switchid, portid));
    EXPECT_EQ(45, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(15, 1, switchid, portid));
    EXPECT_EQ(45, switchid[0]);
    EXPECT_EQ(1, portid[0]);

    // MIN d=0 p={X,1}
    EXPECT_EQ(true, m_pTopology->IsElementToElement(20, 2, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(20, 3, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(0, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(21, 2, switchid, portid));
    EXPECT_EQ(22, switchid[0]);
    EXPECT_EQ(1, portid[0]);
    EXPECT_EQ(true, m_pTopology->IsElementToElement(21, 3, switchid, portid));
    EXPECT_EQ(23, switchid[0]);
    EXPECT_EQ(1, portid[0]);

    // Check the number of elements based on the paper of KNS
    EXPECT_EQ(true, mykns->CheckCalculations());

#ifndef TOPGEN_PBGL    
    std::string dotfile("network.gv");
    m_pTopology->build_bgl();
#endif

    delete m_pTopology;
    delete [] g_pParameter->m_pKaries;
    delete [] g_pParameter->m_pRadixes;
    delete g_pParameter;
}

TEST(bgl, rlft_128n_2l){
    CParameters * g_pParameter = new CParameters();
    g_pParameter->m_eTopology = topgen::rlft;
    g_pParameter->m_iEndNodes = 128;
    g_pParameter->m_iRadix = 16;
    g_pParameter->m_bVerbose = false;
    g_pParameter->m_eConnection = topgen::butterfly;
    g_pParameter->m_eRoutingAlgorithm = topgen::destro;

    topgen::topology * m_pTopology = DefineNewTopology(g_pParameter);
    int endnodes = m_pTopology->NumberOfDestinations();
    EXPECT_EQ(128, endnodes);
    int switches = m_pTopology->NumberOfSwitches();
    EXPECT_EQ(24, switches);
    int outputport = m_pTopology->GetOutputport(0, 1);
    EXPECT_EQ(1, outputport);

    /* Print the routing */
    //for (int i=0; i<switches; ++i) {
    //    for (int j = 0; j < endnodes; ++j) {
    //        int outputport = m_pTopology->GetOutputport(i, j);
    //        //int outputport = ((CRLFT *)m_pTopology)->GetOutputport(i, j);
    //        //printf("Routing sw_id %d dest %d outport %u \n",i,j,outputport);
    //    }
    //}
    
#ifndef TOPGEN_PBGL
    std::string dotfile("network.gv");
    m_pTopology->build_bgl();
#endif
    
    delete m_pTopology;
    delete g_pParameter;
}

int main(int argc, char* argv[]) {
    try {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    } catch (topgen::CException * e) {
        printf("Catched exception from topgen: %s\n", e->Report());
        return EXIT_FAILURE;
    }
}
