/*
 *  KNS.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "topgen/topology.hpp"
#include "topgen/kns.hpp"

namespace topgen {

    CKNS::CKNS(int dimensions, int * karies, knsIndirectType type, int * radixes, bool verbose) : topology() {
        tIndirectType = type;
        m_iChannels = 0;
        m_pChannelList = new std::list<CKNSChannel*>;
        m_iIdentifiers = 0;
        m_iPortToNIC = dimensions; //port from element to IA
        m_iNICPort = 0; //port from IA to element
        m_iDestinations = 0;

        m_pDirect = new CTorusND(dimensions, karies);

        if (type == knsmin) {
            m_pIndirect = new CMIN*[dimensions];
            for (int d = 0; d < dimensions; d++) {
                m_pIndirect[d] = new CMIN(karies[d], radixes[d], butterfly, destro);
            }
        } else if (type == knsrouter) {
            m_pIndirectRouter = new CRouter*[dimensions];

            for (int d = 0; d < dimensions; d++) {
                m_pIndirectRouter[d] = new CRouter(radixes[d]);
            }
        } else {
            throw new CException("CKNS: CKNS(): indirect topology unknown");
        }

        m_pPortToDimension = new int[dimensions];
        m_pVirtRouters = new std::list<CElementType_Node*>();
        m_pVirtSwitches = new std::list<CElementType_Node*>();

        // db to store virtual routers classified by dim x pos
        m_pMINs = new std::list<struct stMIN*>*[m_pDirect->GetDimensions()];
        for (int dim = 0; dim < m_pDirect->GetDimensions(); dim++) {
            m_pMINs[dim] = new std::list<struct stMIN*>();
        }

        m_iElementCategories = 1 + dimensions;
    }

    CKNS::~CKNS() {
        for (int dim = 0; dim < m_pDirect->GetDimensions(); dim++) {
            unsigned long int size = m_pMINs[dim]->size();
            while (size > 0) {
                struct stMIN * vmin = m_pMINs[dim]->front();
                m_pMINs[dim]->pop_front();
                EmptyMIN(vmin);
                size--;
            }
            delete m_pMINs[dim];
            if (tIndirectType == knsmin)
                delete m_pIndirect[dim];
            else if (tIndirectType == knsrouter)
                delete m_pIndirectRouter[dim];
        }
        delete [] m_pMINs;
        if (tIndirectType == knsmin)
            delete [] m_pIndirect;
        else if (tIndirectType == knsrouter)
            delete [] m_pIndirectRouter;

        delete m_pDirect;

        delete [] m_pPortToDimension;
        delete m_pVirtRouters;
        delete m_pVirtSwitches;

        std::list<CKNSChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            CKNSChannel * ch = (*it);
            delete ch;
            ++it;
        }
        delete m_pChannelList;

        _freeMemory();
    }

    void CKNS::RunAll() {
        BuildNetwork();
        BuildInterconnection();
        BuildChannels();
        LoadNetwork();
        Vectorize();
        PreBuildRoutes();
        BuildRoutes();
        LoadRoutes();
    }

    void CKNS::BuildNetwork() {
        // initialization of the direct network
        m_pDirect->RunAll();

        // initialization of the indirect networks
#pragma omp parallel for
        for (int d = 0; d < m_pDirect->GetDimensions(); d++) {
            int indirect_nodes = -1;
            if (tIndirectType == knsmin) {
                m_pIndirect[d]->RunAll();
                indirect_nodes = m_pIndirect[d]->GetNodes();
            } else if (tIndirectType == knsrouter) {
                m_pIndirectRouter[d]->RunAll();
                indirect_nodes = m_pIndirectRouter[d]->GetNodes();
            } else
                throw new CException("CKNS: BuildNetwork(): indirect topology unknown");

            // little checking of the numbers
            if (m_pDirect->GetKary(d + 1) > indirect_nodes) {
                char * errmsg = new char[256];
                snprintf(errmsg, 256, "CKNS: BuildNetwork(): Check parameters: "
                                "Mismatch at dimension %d, direct has %d IAs, but indirect %d IAs)."
                                ,d + 1, m_pDirect->GetKary(d + 1), indirect_nodes);
                throw new CException(errmsg);
            }
        }
    }

    void CKNS::BuildRoutes() {
        // nothing to do here
    }

    void CKNS::BuildInterconnection() {
        //nothing to do here
    }

    void CKNS::BuildChannels() {
        // nothing to do here
    }

    int CKNS::GetNumberOfSwitches() {

        return m_iNodes;
    }

    int CKNS::GetNumberOfDestinations() {

        return m_iDestinations;
    }

    int CKNS::GetNodes() {

        return m_iNodes;
    }

    void CKNS::LoadNetwork() {
        // load of the direct network
        CInputAdapterType_Node * new_ia;
        CInputAdapterType_Node * the_ia;
        CElementType_Node * cur_element;
        CElementType_Node * new_element;
        CKNSChannel * ch;
        char * name = new char[TOPGEN_MAX_STRING_LENGTH];

        // 1) load of direct network
        int direct_coordinates_length = m_pDirect->GetDimensions();
        for (int idx = 0; idx < m_pDirect->GetNodes(); idx++) {
            // 1.a) IAs
            int * direct_coordinates = m_pDirect->GetCoordinates(idx);

            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia id %d", m_iDestinations);
            new_ia = new CInputAdapterType_Node("nic",
                name,
                m_iDestinations,
                "");

            new_ia->WriteTuple(direct_coordinates_length, direct_coordinates);
            delete [] direct_coordinates;
            m_pInputAdapters->push_back(new_ia);
            m_iDestinations++;

            // 1.b) Routers
            cur_element = m_pDirect->GetElement(idx);
            direct_coordinates = m_pDirect->GetCoordinates(idx);

            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "router id %d", m_iIdentifiers);
            new_element = new CElementType_Node("router",
                name,
                m_iIdentifiers,
                cur_element->m_iPorts + 1, // Add 1 due to the NIC of the KNS
                0);

            new_element->WriteTuple(direct_coordinates_length, direct_coordinates);
            delete [] direct_coordinates;
            m_pVirtRouters->push_back(new_element);
            m_pElements->push_back(new_element);
            m_iIdentifiers++;


            // 1.c) channels between the switches of the direct topology and IAs
            the_ia = NULL;
            std::list<CInputAdapterType_Node*>::iterator it1;
            it1 = m_pInputAdapters->begin();
            while (it1 != m_pInputAdapters->end()) {
                if ((*it1)->m_iID == idx) {
                    the_ia = (*it1);
                    break;
                }
                ++it1;
            }

            if (the_ia == NULL)
                throw new CException("CKNS:LoadNetwork(): NULL IA found");

            ch = new CKNSChannel(iaSTD_kns,
                m_iChannels,
                new_element, // down
                m_iPortToNIC,
                the_ia, // up
                m_iNICPort);
            m_iChannels++;

            m_pChannelList->push_back(ch);
        }

        // 2) load of the indirect networks
        // 2.1) classification of the routers by VMINs
        for (int dim = 0; dim < m_pDirect->GetDimensions(); dim++) {
            std::list<CElementType_Node*>::iterator it2;
            for (it2 = m_pVirtRouters->begin(); it2 != m_pVirtRouters->end(); ++it2) {
                CElementType_Node * vr = (*it2);
                struct stMIN * vmin = GetRefMIN(dim, vr);
                //add the reference to virtual router to the list of the MIN
                vmin->vrouters.push_back(vr);
            }
        }

        // 2.2) building of virtual switches based-on templates
        for (int dim = 0; dim < m_pDirect->GetDimensions(); dim++) {
            std::list<struct stMIN*>::iterator it3;
            for (it3 = m_pMINs[dim]->begin(); it3 != m_pMINs[dim]->end(); ++it3) {
                // process every MIN called vmin according to the dimension
                struct stMIN * vmin = (*it3);

                // now we have the correct vmin->position
                // so we can read template of indirect networks
                // so that we build the virtual switches
                if (tIndirectType == knsmin) {
                    for (int idx = 0; idx < m_pIndirect[dim]->GetNumberOfSwitches(); idx++) {
                        cur_element = m_pIndirect[dim]->GetElement(idx);
                        int * indirect_coordinates = m_pIndirect[dim]->GetCoordinates(idx);
                        int indirect_stages = m_pIndirect[dim]->GetStages();

                        snprintf(name, TOPGEN_MAX_STRING_LENGTH, "switch id %d", m_iIdentifiers);
                        new_element = new CElementType_Node("switch",
                            name,
                            m_iIdentifiers,
                            indirect_coordinates[0] == indirect_stages - 1, // this coordinates stores de stage
                            cur_element->m_iPorts, // keep the ports from the MIN
                            dim + 1, //dimension+1 stands for the category (0 for direct routers)
                            cur_element); // keep a reference to the template node


                        new_element->WriteTuple(dim, vmin->position, direct_coordinates_length, indirect_coordinates);
                        delete [] indirect_coordinates;
                        m_pVirtSwitches->push_back(new_element);
                        m_pElements->push_back(new_element);
                        vmin->vswitches.push_back(new_element); // register the virtual switch
                        vmin->tswitches.push_back(cur_element); // register the template switch
                        m_iIdentifiers++;
                    }
                } else if (tIndirectType == knsrouter) {
                    cur_element = m_pIndirectRouter[dim]->GetElement(0); //the unique
                    int * indirect_coordinates = m_pIndirectRouter[dim]->GetCoordinates(0);
                    snprintf(name, TOPGEN_MAX_STRING_LENGTH, "switch id %d", m_iIdentifiers);
                    new_element = new CElementType_Node("switch",
                        name,
                        m_iIdentifiers,
                        true, // it has no sense since there's one switch
                        cur_element->m_iPorts, // keep the ports from the switch
                        dim + 1, //dimension+1 stands for the category (0 for direct routers)
                        cur_element); // keep a reference to the template node

                    new_element->WriteTuple(dim, vmin->position, direct_coordinates_length, indirect_coordinates);
                    delete [] indirect_coordinates;
                    m_pVirtSwitches->push_back(new_element);
                    m_pElements->push_back(new_element);
                    vmin->vswitches.push_back(new_element); // register the virtual switch
                    vmin->tswitches.push_back(cur_element); // register the template switch
                    m_iIdentifiers++;

                } else {
                    throw new CException("CKNS: LoadNetwork(): indirect topology unknown");
                }

            }
        }

        // 3) creating of channels between:
        //    routers of the direct topology and
        //    switches of the indirect topology
        for (int dim = 0; dim < m_pDirect->GetDimensions(); dim++) {
            if (tIndirectType == knsmin) {
                const int local_k = m_pIndirect[dim]->GetSidePorts();

                std::list<struct stMIN*>::iterator it3;
                for (it3 = m_pMINs[dim]->begin(); it3 != m_pMINs[dim]->end(); ++it3) {
                    struct stMIN * vmin = (*it3);

                    // 3.1) channels routers-switches
                    for (int local_dst = 0; local_dst < m_pDirect->GetKary(dim + 1); local_dst++) {
                        bool down_vr_found = false;
                        bool up_vs_found = false;
                        CElementType_Node * down_vr = NULL;
                        CElementType_Node * up_vs = NULL;

                        // search down element
                        std::list<CElementType_Node*>::iterator it4;
                        for (it4 = vmin->vrouters.begin(); it4 != vmin->vrouters.end(); ++it4) {
                            down_vr = (*it4);

                            if (down_vr->m_pCoordinates[dim] == local_dst) {
                                down_vr_found = true;
                                break;
                            }
                        }

                        // search up element
                        //printf("local_dst %d\n", local_dst);
                        int local_sw = local_dst / local_k;
                        int local_port = local_dst % local_k;
                        std::list<CElementType_Node*>::iterator it5;
                        for (it5 = vmin->vswitches.begin(); it5 != vmin->vswitches.end(); ++it5) {
                            up_vs = (*it5);

                            // stage 0 & switch inside stage 0
                            if (up_vs->m_pCoordinates[0] == 0 &&
                                up_vs->m_pCoordinates[1] == local_sw) {
                                up_vs_found = true;
                                break;
                            }
                        }

                        if (!down_vr_found || !up_vs_found)
                            throw new CException("CKNS::LoadNetwork(): Unknown virtual elements (MIN case)");

                        char prefix[200];
                        snprintf(prefix, 200, "VR: %s coordinates [", down_vr->m_sName);
                        snprintf(prefix, 200, "VS: %s position [", up_vs->m_sName);

                        CKNSChannel * ch = new CKNSChannel(virtSTD_kns,
                            m_iChannels, //nch->m_iID,
                            down_vr,
                            dim,
                            up_vs,
                            local_port,
                            dim,
                            vmin->position,
                            m_pDirect->GetDimensions(),
                            m_iChannels);
                        m_iChannels++;
                        m_pChannelList->push_back(ch);
                    }

                    // 3.2) build the channels of the indirect templates renaming them
                    std::list<CMINChannel*> * template_chls = m_pIndirect[dim]->GetNodeChannels();
                    std::list<CElementType_Node*>::iterator it6;
                    for (it6 = vmin->vswitches.begin(); it6 != vmin->vswitches.end(); ++it6) {
                        CElementType_Node * vs1 = (*it6);
                        CElementType_Node * vs2 = NULL;
                        int vs1_port;
                        int vs2_port = -1;

                        // omit switches at stage 0
                        if (vs1->m_pCoordinates[0] == 0)
                            continue;

                        for (vs1_port = 0; vs1_port < local_k; vs1_port++) {
                            bool vs1_is_down = false;
                            bool vs1_is_up = false;
                            bool vs2_found = false;

                            std::list<CMINChannel*>::iterator it7;
                            for (it7 = template_chls->begin(); it7 != template_chls->end(); ++it7) {
                                CMINChannel * rch = (*it7);

                                if (vs1->m_pTemplateNode->m_iIdentifier == rch->m_pUpNode->GetIdentifier()
                                    && vs1_port == rch->m_pUpPort->port) {
                                    // channel direction 1
                                    vs1_is_up = true;

                                    //vs2
                                    std::list<CElementType_Node*>::iterator it9;
                                    for (it9 = vmin->vswitches.begin(); it9 != vmin->vswitches.end(); ++it9) {
                                        vs2 = (*it9);

                                        if (vs2->m_pTemplateNode->m_iIdentifier == rch->m_pDownNode->GetIdentifier()) {
                                            vs2_found = true;
                                            vs2_port = rch->m_pDownPort->port;
                                            break;
                                        }
                                    }
                                } else if (vs1->m_pTemplateNode->m_iIdentifier == rch->m_pDownNode->GetIdentifier()
                                    && vs1_port == rch->m_pDownPort->port) {
                                    // channel direction 2
                                    vs1_is_down = true;

                                    //vs2
                                    std::list<CElementType_Node*>::iterator it8;
                                    for (it8 = vmin->vswitches.begin(); it8 != vmin->vswitches.end(); ++it8) {
                                        vs2 = (*it8);

                                        if (vs2->m_pTemplateNode->m_iIdentifier == rch->m_pUpNode->GetIdentifier()) {
                                            vs2_found = true;
                                            vs2_port = rch->m_pUpPort->port;
                                            break;
                                        }
                                    }
                                }

                                if ((vs1_is_up && vs2_found) || (vs1_is_down && vs2_found)) {
                                    // channel matched
                                    CKNSChannel * ch = new CKNSChannel(virtSTD_kns,
                                        m_iChannels, //nch->m_iID,
                                        vs1,
                                        vs1_port,
                                        vs2,
                                        vs2_port,
                                        dim,
                                        vmin->position,
                                        m_pDirect->GetDimensions(),
                                        m_iChannels);
                                    m_iChannels++;
                                    m_pChannelList->push_back(ch);
                                    // this real-channel is erased from the list
                                    // prevent executing it7++ with break
                                    template_chls->erase(it7);
                                    break; // next vs1_port
                                }
                            }
                        }
                    }
                    template_chls->clear();
                    delete template_chls;
                }
            } else if (tIndirectType == knsrouter) {
                std::list<struct stMIN*>::iterator it3;
                for (it3 = m_pMINs[dim]->begin(); it3 != m_pMINs[dim]->end(); ++it3) {
                    struct stMIN * vmin = (*it3);

                    // 3.1) channels routers-switches
                    for (int local_dst = 0; local_dst < m_pDirect->GetKary(dim + 1); local_dst++) {
                        bool down_vr_found = false;
                        bool up_vs_found = false;
                        CElementType_Node * down_vr = NULL;
                        CElementType_Node * up_vs = NULL;

                        // search down element (virtual router)
                        std::list<CElementType_Node*>::iterator it4;
                        for (it4 = vmin->vrouters.begin(); it4 != vmin->vrouters.end(); ++it4) {
                            down_vr = (*it4);

                            if (down_vr->m_pCoordinates[dim] == local_dst) {
                                down_vr_found = true;
                                break;
                            }
                        }

                        // search up element (the unique router)
                        up_vs = vmin->vswitches.front();
                        if (up_vs != NULL) {
                            up_vs_found = true;
                        }
                        
                        int local_port = local_dst;

                        if (!down_vr_found || !up_vs_found)
                            throw new CException("CKNS::LoadNetwork(): Unknown virtual elements (router case)");

                        char prefix[200];
                        snprintf(prefix, 200, "VR: %s coordinates ", down_vr->m_sName);
                        snprintf(prefix, 200, "VS: %s position ", up_vs->m_sName);

                        CKNSChannel * ch = new CKNSChannel(virtSTD_kns,
                            m_iChannels, //nch->m_iID,
                            down_vr,
                            dim,
                            up_vs,
                            local_port,
                            dim,
                            vmin->position,
                            direct_coordinates_length,
                            m_iChannels);
                        m_iChannels++;
                        m_pChannelList->push_back(ch);
                    }

                    // 3.2) build the channels of the indirect templates renaming them
                    // nothing to do here
                }
            } else {
                throw new CException("CKNS: LoadNetwork(): indirect topology unknown");
            }
        }

        // Channels
        std::list<CKNSChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create());
            ++it;
        }

        // free space
        delete [] name;
    }

    void CKNS::LoadRoutes() {
        // nothing to do here
    }

    int CKNS::GetPortToNIC() {

        return m_iPortToNIC;
    }

    int CKNS::GetNICPort() {

        return m_iNICPort;
    }

    int CKNS::GetPortToDimension(const int dimension) {

        return m_pPortToDimension[dimension];
    }

    int CKNS::GetStagesDimension(const int d) {

        return m_pIndirect[d]->GetStages();
    }

    CElementType_Node * CKNS::GetVirtRouter(int * coordinates) {
        std::list<CElementType_Node*>::iterator it;
        it = m_pVirtRouters->begin();

        while (it != m_pVirtRouters->end()) {
            CElementType_Node * virtual_router = (*it);
            bool match = true;
            for (int d = 0; d < m_pDirect->GetDimensions(); d++) {
                if (virtual_router->m_pCoordinates[d] != coordinates[d]) {
                    match = false;
                    break;
                }
            }

            if (match == true) {
                return virtual_router;
            }

            ++it;
        }

        return NULL;
    }

    // search the min, create if doesn't exist

    struct stMIN * CKNS::GetRefMIN(int dimension, CElementType_Node * vr) {
        struct stMIN * min = NULL;
        bool found = false;

        std::list<struct stMIN*>::iterator it;
        for (it = m_pMINs[dimension]->begin(); it != m_pMINs[dimension]->end(); ++it) {
            min = (*it);

            //search
            bool match = true;
            for (int i = 0; i < m_pDirect->GetDimensions(); i++) {
                if (i == dimension)//ignore this dimension
                    continue;

                if (vr->m_pCoordinates[i] != min->position[i]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                //position matches
                found = true;
                break;
            }
        }

        if (!found) {
            min = new struct stMIN[1];
            min->position = new int[m_pDirect->GetDimensions()];
            for (int i = 0; i < m_pDirect->GetDimensions(); i++) {

                min->position[i] = vr->m_pCoordinates[i]; // copy all the coords
            }

            // store
            m_pMINs[dimension]->push_back(min);
        }

        return min;
    }

    void CKNS::EmptyMIN(struct stMIN * min) {
        if (min->position)
            delete [] min->position;
        delete [] min;
    }

    bool CKNS::CheckCalculations() {
        return true; // FIXME
    }

} // namespace topology
