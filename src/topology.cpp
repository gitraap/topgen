/*
 *  BaseNetwork.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include "topgen/topology.hpp"
#include "topgen/hu_channel.hpp"
#include "topgen/xgft_channel.hpp"
#include "topgen/dir_channel.hpp"
#include "topgen/min_channel.hpp"
#include "topgen/kns_channel.hpp"
#include "topgen/rlft_channel.hpp"
#include "topgen/dfly_channel.hpp"

#ifndef TOPGEN_PBGL
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/graph_utility.hpp> // print_graph
#include <boost/property_map/property_map.hpp> //
//#include <boost/config.hpp>

#else

// Enable PBGL interfaces to BGL algorithms
#include <boost/graph/use_mpi.hpp>

//#include <boost/mpi/environment.hpp>
//#include <boost/mpi/communicator.hpp>
//#include <boost/mpi/status.hpp>

// Communication via MPI
#include <boost/graph/distributed/mpi_process_group.hpp>

// Dijkstra's single-source shortest paths algorithm
//#include <boost/graph/dijkstra_shortest_paths.hpp>

// Distributed adjacency list
//#include <boost/graph/distributed/adjacency_list.hpp>

// Graphviz Output
//#include <boost/graph/distributed/graphviz.hpp>

#endif


namespace topgen {

    topology::topology() {
        m_pInputAdaptersVector = NULL;
        m_pElementsVector = NULL;
        m_iChannelsArrayLen = 150; // by default: we only support switches with 100 ports
        m_pChannelsArray = new std::list<CChannelType_Node*>*[m_iChannelsArrayLen];
        for (int i = 0; i < m_iChannelsArrayLen; i++) {
            m_pChannelsArray[i] = new std::list<CChannelType_Node*>();
        }
        m_pRoutesArray = NULL;

        m_pInputAdapters = new std::list<CInputAdapterType_Node*>;
        m_pElements = new std::list<CElementType_Node*>;
        m_pChannels = new std::list<CChannelType_Node*>;
        m_pRoutes = new std::list<CRoute_Node*>;
        m_iElementCategories = 1; // by default, all the elements belong to the same category
        m_pMemoryBreakdown.dInputAdapters = 0.0;
        m_pMemoryBreakdown.dElements = 0.0;
        m_pMemoryBreakdown.dChannels = 0.0;
        m_pMemoryBreakdown.dRouters = 0.0;
    }

    void topology::_freeMemory() {
#pragma omp parallel
        {
#pragma omp sections
            {
#pragma omp section
                {
                    if (m_pRoutesArray != NULL) {
                        m_iSwitchIdentifiers = NumberOfSwitches();
                        for (int sw = 0; sw < m_iSwitchIdentifiers; sw++) {
                            assert(m_pRoutesArray[sw] != NULL);
                            delete [] m_pRoutesArray[sw];
                        }
                        delete [] m_pRoutesArray;
                    }
                }

#pragma omp section
                {
                    CInputAdapterType_Node * ianode;
                    while (!m_pInputAdapters->empty()) {
                        ianode = m_pInputAdapters->front();
                        m_pInputAdapters->pop_front();
                        delete ianode;
                    }
                    delete m_pInputAdapters;
                }

#pragma omp section
                {
                    CElementType_Node * enode;
                    while (!m_pElements->empty()) {
                        enode = m_pElements->front();
                        m_pElements->pop_front();
                        delete enode;
                    }
                    delete m_pElements;
                }

#pragma omp section
                {            
                    CChannelType_Node * ch;
                    while (!m_pChannels->empty()) {
                        ch = m_pChannels->front();
                        m_pChannels->pop_front();
                        delete ch;
                    }
                    delete m_pChannels;
                }

#pragma omp section
                {            
                    CRoute_Node * rnode;
                    while (!m_pRoutes->empty()) {
                        rnode = m_pRoutes->front();
                        m_pRoutes->pop_front();
                        if (rnode)
                            delete rnode;
                    }
                    delete m_pRoutes;
                }

#pragma omp section
                {            
                    if (m_pInputAdaptersVector)
                        delete [] m_pInputAdaptersVector;
                    if (m_pElementsVector)
                        delete [] m_pElementsVector;

                    // Remind that the nodes m_pChannelsArray[i] are destroyed by ~CChannelType_Node()
                    // Here, just empty the lists m_pChannelsArray[i];
                    for (int i = 0; i < m_iChannelsArrayLen; i++) {
                        delete m_pChannelsArray[i];
                    }
                    delete [] m_pChannelsArray;
                }
            }
        }
    }

    void topology::Vectorize() {
#pragma omp parallel
        {
#pragma omp sections
            {
                printf("+++ vectorizing with %u threads.\n", omp_get_num_threads());
#pragma omp section
                {
                    // Input Adapters
                    m_iInputAdapters = m_pInputAdapters->size();
                    m_pInputAdaptersVector = new CInputAdapterType_Node*[m_iInputAdapters];

                    std::list<CInputAdapterType_Node*>::iterator it1;
                    for (it1 = m_pInputAdapters->begin(); it1 != m_pInputAdapters->end(); ++it1) {
                        assert((*it1)->m_iID < m_iInputAdapters);
                        m_pInputAdaptersVector[(*it1)->m_iID] = (*it1);
                    }
                }

#pragma omp section
                {            
                    // Elements
                    m_iElements = m_pElements->size();
                    m_pElementsVector = new CElementType_Node*[m_iElements];
                    for (int el = 0; el < m_iElements; el++) {
                        assert(el < m_iElements);
                        m_pElementsVector[el] = NULL;
                    }

                    std::list<CElementType_Node*>::iterator it2;
                    for (it2 = m_pElements->begin(); it2 != m_pElements->end(); ++it2) {
                        m_pElementsVector[(*it2)->m_iIdentifier] = (*it2);
                    }
                }

#pragma omp section
                {
                    // Channels
                    m_iChannels = m_pChannels->size();
                    std::list<CChannelType_Node*>::iterator it3 = m_pChannels->begin();
                    int size = m_iChannels;
                    while (size > 0) {
                        CChannelType_Node * item = (*it3);
                        assert(atoi(item->m_sDownPort) == item->m_iDownPort);
                        assert(atoi(item->m_sUpPort) == item->m_iUpPort);
                        int dpt = item->m_iDownPort;
                        int upt = item->m_iUpPort;
                        // Note that item is stored twice, keep this in mind in the destructor
                        // so never delete them because CChannelType_Node destructor will do
                        // item is just a copy
                        m_pChannelsArray[dpt]->push_back(item);
                        m_pChannelsArray[upt]->push_back(item);
                        ++it3;
                        --size;
                    }
                }
            }
        }
    }

    void topology::PreBuildRoutes() {

    }

    void topology::PostVectorize() {
        // Routes
        m_iSwitchIdentifiers = NumberOfSwitches();
        m_iNetworkDestinations = NumberOfDestinations();
        m_pRoutesArray = new int*[m_iSwitchIdentifiers];
        for (int sw = 0; sw < m_iSwitchIdentifiers; sw++) {
            m_pRoutesArray[sw] = new int[m_iNetworkDestinations];
            for (int dst = 0; dst < m_iNetworkDestinations; dst++) {
                m_pRoutesArray[sw][dst] = -1;
            }
        }

        int sw, dst, oport;
        std::list<CRoute_Node*>::iterator it;
        for (it = m_pRoutes->begin(); it != m_pRoutes->end(); ++it) {
            sw = (*it)->m_iNodeID;
            dst = (*it)->m_iDestination;
            oport = (*it)->m_iOutputPort;
            m_pRoutesArray[sw][dst] = oport;
        }
    }

    void topology::_LoadChannel(int selector, const char * type, int id, int upNode, int upPort, int downNode, int downPort) {
        char * sid = new char[TOPGEN_MAX_STRING_LENGTH];
        char * upnode = new char[TOPGEN_MAX_STRING_LENGTH];
        char * upport = new char[TOPGEN_MAX_STRING_LENGTH];
        char * downnode = new char[TOPGEN_MAX_STRING_LENGTH];
        char * downport = new char[TOPGEN_MAX_STRING_LENGTH];

        CChannelType_Node * new_channel;
        if (selector == 0) {
            // network channel
            sprintf(sid, "%d", id);
            sprintf(upnode, "s%d", upNode);
            sprintf(upport, "%d", upPort);
            sprintf(downnode, "s%d", downNode);
            sprintf(downport, "%d", downPort);
            new_channel = new CChannelType_Node(type, sid, id, upnode, upport, upNode, upPort, downnode, downport, downNode, downPort);
        } else if (selector == 1) {
            // network <--> NIC
            sprintf(sid, "%d", id);
            sprintf(upnode, "s%d", upNode);
            sprintf(upport, "%d", upPort);
            sprintf(downnode, "ia%d", downNode);
            sprintf(downport, "%d", downPort);
            new_channel = new CChannelType_Node(type, sid, id, upnode, upport, upNode, upPort, downnode, downport, downNode, downPort);
        } else throw new CException("CChannel:_LoadChannel(): Illegal selector type");

        delete [] sid;
        delete [] upnode;
        delete [] upport;
        delete [] downnode;
        delete [] downport;

        m_pChannels->push_back(new_channel);
    }
    void topology::_LoadChannel(CChannelType_Node *channel) {
        m_pChannels->push_back(channel);
    }

    void topology::_LoadRoute(int identifier, int destination, int output) {
        CRoute_Node * route = new CRoute_Node(identifier, destination, 1, output);
        m_pRoutes->push_back(route);
    }

    void topology::_LoadIA(CInputAdapterType_Node * ia) {
        m_pInputAdapters->push_back(ia);
    }

    void topology::_LoadElement(CElementType_Node * element) {
        m_pElements->push_back(element);
    }

    int topology::NumberOfDestinations() {
        return m_pInputAdapters->size();
    }

    int topology::NumberOfSwitches() {
        return m_pElements->size();
    }

    int topology::GetElementIdentifier(int index) {
        if (m_pElementsVector[index]->m_iIdentifier == index) {
            return index;
        }
        return -1;
    }

    int topology::GetInputAdapterIdentifier(int index) {
        int position = 0;
        std::list<CInputAdapterType_Node*>::iterator it;
        for (it = m_pInputAdapters->begin(); it != m_pInputAdapters->end(); ++it) {
            if (position == index) {
                return (*it)->m_iID;
            }

            ++position;
        }

        return -1;
    }

    CElementType_Node * topology::GetElement(int identifier) {
        return m_pElementsVector[identifier];
    }

    CInputAdapterType_Node * topology::GetInputAdapter(int identifier) {
        return m_pInputAdaptersVector[identifier];
    }

    int topology::GetNumberOfPorts(int identifier) {
        return (int) m_pElementsVector[identifier]->m_iPorts;
    }

    int topology::GetElementConnectedTo(int ia_id) {
        CInputAdapterType_Node * ia = GetInputAdapter(ia_id); // element is an input adapter
        if (!ia)
            throw new CException("topology: GetElementConnectedTo(): NULL Input Adapter");

        CChannelType_Node * channel = GetChannel(ia->m_sName, 0);

        if (channel == NULL) {
            throw new CException("topology: GetElementConnectedTo(): NULL Channel");
        }
        if (!strcmp(channel->m_sDownElement, ia->m_sName) && channel->m_iDownPort == 0) {
            return GetElementIdentifier(channel->m_sUpElement);
        } else if (!strcmp(channel->m_sUpElement, ia->m_sName) && channel->m_iUpPort == 0) {
            return GetElementIdentifier(channel->m_sDownElement);
        }

        throw new CException("topology: GetElementConnectedTo(): Not found. is it wrong?");
        return -1;
    }

    CChannelType_Node * topology::GetChannel(const char * name, int port) {
        std::list<CChannelType_Node*>::iterator it = m_pChannelsArray[port]->begin();
        unsigned size = m_pChannelsArray[port]->size();
        for (unsigned ch = 0; ch < size; ch++) {
            CChannelType_Node* item = (*it);

            if (!strcmp(item->m_sDownElement, name)) {
                if (item->m_iDownPort == port) {
                    return item;
                }
            }
            if (!strcmp(item->m_sUpElement, name)) {
                if (item->m_iUpPort == port) {
                    return item;
                }
            }

            ++it;
        }

        return NULL;
    }

    int topology::GetElementIdentifier(const char * name) {
        // element
        std::list<CElementType_Node*>::iterator it1 = m_pElements->begin();
        for (unsigned elements = m_pElements->size(); elements > 0; elements--) {
            if (!strcmp((*it1)->m_sName, name)) {
                return (*it1)->m_iIdentifier;
            }
            ++it1;
        }

        return -1;
    }

    int topology::GetInputAdapterIdentifier(const char * name) {
        // input adapters
        std::list<CInputAdapterType_Node*>::iterator it3 = m_pInputAdapters->begin();
        for (unsigned ias = m_pInputAdapters->size(); ias > 0; ias--) {
            if (!strcmp((*it3)->m_sName, name)) {
                return (*it3)->m_iID;
            }
            ++it3;
        }

        return -1;
    }

    int topology::GetChannelIdentifier(const char * name) {
        // channels
        std::list<CChannelType_Node*>::iterator it2 = m_pChannels->begin();
        for (unsigned chs = m_pChannels->size(); chs > 0; chs--) {
            if (!strcmp((*it2)->m_sName, name)) {
                return atoi((*it2)->m_sIdentifier);
            }
            ++it2;
        }

        return -1;
    }

    int topology::GetIdentifier(const char * name) {
        int element_id = GetElementIdentifier(name);

        if (element_id != -1)
            return element_id;

        int ia_id = GetInputAdapterIdentifier(name);

        if (ia_id != -1)
            return ia_id;

        int channel_id = GetChannelIdentifier(name);

        if (channel_id != -1)
            return channel_id;

        return -1;
    }

    bool topology::IsElementToIA(int node_id, int link_id, unsigned int * next_ia_id, unsigned int * next_ia_port) {
        CElementType_Node * element = GetElement(node_id); // element is a switch
        CChannelType_Node * channel = GetChannel(element->m_sName, link_id);

        if (element == NULL || channel == NULL) {
            *next_ia_id = UINT_MAX;
            *next_ia_port = UINT_MAX;
            return false;
        }

        int id;

        if (!strcmp(channel->m_sDownElement, element->m_sName) && channel->m_iDownPort == link_id) {
            id = GetInputAdapterIdentifier(channel->m_sUpElement);
            if (id != -1) {
                *next_ia_id = id;
                *next_ia_port = channel->m_iUpPort;
                return true;
            }
        } else if (!strcmp(channel->m_sUpElement, element->m_sName) && channel->m_iUpPort == link_id) {
            id = GetInputAdapterIdentifier(channel->m_sDownElement);
            if (id != -1) {
                *next_ia_id = id;
                *next_ia_port = channel->m_iDownPort;
                return true;
            }
        }

        return false;
    }

    bool topology::IsElementToElement(int node_id, int link_id, unsigned int * next_node_id, unsigned int * next_node_port) {
        CElementType_Node * element = GetElement(node_id); // element is a switch
        CChannelType_Node * channel = GetChannel(element->m_sName, link_id);

        if (element == NULL || channel == NULL) {
            *next_node_id = UINT_MAX;
            *next_node_port = UINT_MAX;
            return false;
        }

        int id;

        if (channel->m_iDownPort == link_id) {
            if (!strcmp(channel->m_sDownElement, element->m_sName)) {
                id = GetElementIdentifier(channel->m_sUpElement);
                if (id != -1) {
                    *next_node_id = id;
                    *next_node_port = channel->m_iUpPort;
                    return true;
                }
            }
        }

        if (channel->m_iUpPort == link_id) {
            if (!strcmp(channel->m_sUpElement, element->m_sName)) {
                id = GetElementIdentifier(channel->m_sDownElement);
                if (id != -1) {
                    *next_node_id = id;
                    *next_node_port = channel->m_iDownPort;
                    return true;
                }
            }
        }

        return false;
    }

    bool topology::IsElementToAir(int node_id, int link_id, unsigned int * next_node_id, unsigned int * next_node_port) {
        CElementType_Node * element = GetElement(node_id); // element is a switch
        CChannelType_Node * channel = GetChannel(element->m_sName, link_id);

        if (element == NULL || channel == NULL) {
            *next_node_id = UINT_MAX;
            *next_node_port = UINT_MAX;
            return true;
        }

        return false;
    }

    bool topology::IsIAToElement(int node_id, int link_id, unsigned int * next_node_id, unsigned int * next_node_port) {
        CInputAdapterType_Node * inputadapter = GetInputAdapter(node_id); // element is an input adapter
        CChannelType_Node * channel = GetChannel(inputadapter->m_sName, link_id);

        if (inputadapter == NULL || channel == NULL) {
            *next_node_id = UINT_MAX;
            *next_node_port = UINT_MAX;
            return false;
        }

        int id;

        if (!strcmp(channel->m_sDownElement, inputadapter->m_sName) && channel->m_iDownPort == link_id) {
            id = GetElementIdentifier(channel->m_sUpElement);
            if (id != -1) {
                *next_node_id = id;
                *next_node_port = channel->m_iUpPort;
                return true;
            }
        } else if (!strcmp(channel->m_sUpElement, inputadapter->m_sName) && channel->m_iUpPort == link_id) {
            id = GetElementIdentifier(channel->m_sDownElement);
            if (id != -1) {
                *next_node_id = id;
                *next_node_port = channel->m_iDownPort;
                return true;
            }
        }

        return false;
    }

    bool topology::IsExternalChannel(int node_id, int link_id) {
        CElementType_Node * element = GetElement(node_id); // element is a switch
        CChannelType_Node * channel = GetChannel(element->m_sName, link_id);

        if (!strcmp(channel->m_sType, "ext")) {
            return true;
        } else if (!strcmp(channel->m_sType, "int")) {
            return false;
        }

        throw new CException("BaseNetwork: IsExternalChannel(): Unknown type");
        return false;
    }

    int topology::GetOutputport(int identifier, int destination) {
        if (m_pRoutesArray[identifier][destination] != -1)
            return m_pRoutesArray[identifier][destination];

        return UINT_MAX;
    }

    int topology::MyLog(int number, int base) {
        int times = 0;
        int acumulator = 1;

        while (acumulator < number) {
            acumulator *= base;
            times++;
        }

        return times;
    }

    int topology::GetElementCategories() {
        return m_iElementCategories;
    }

    long long unsigned topology::GetMemoryOccupancy() {
        long long unsigned total_bytes = 0;

        // THIS OBJECT
        total_bytes += sizeof (this);
        // TODO complete the calculations recursively with objects

        // Note: std::list is a double-linked list so there are 3 pointers:
        // to the object + to the previous object + to the next object
        m_pMemoryBreakdown.dInputAdapters = m_iInputAdapters * sizeof (CInputAdapterType_Node);
        m_pMemoryBreakdown.dElements = m_iElements * sizeof (CElementType_Node);
        m_pMemoryBreakdown.dChannels = m_iChannels * sizeof (CChannelType_Node);

        total_bytes += m_pMemoryBreakdown.dInputAdapters;
        total_bytes += m_pMemoryBreakdown.dElements;
        total_bytes += m_pMemoryBreakdown.dChannels;

        return total_bytes;
    }

    void topology::printTopology() {
        printf("Network nodes: %lu\n", m_pInputAdapters->size());

        std::list<CInputAdapterType_Node*>::iterator it1;
        for (it1 = m_pInputAdapters->begin(); it1 != m_pInputAdapters->end(); it1++) {
            CInputAdapterType_Node * aux = (*it1);
            printf("HCA: %d, Name: %s, Type: %s, Value:%s\n",
                    aux->m_iID, aux->m_sName, aux->m_sType, aux->m_sValue);
        }

        printf("Network Switches: %lu\n", m_pElements->size());

        std::list<CElementType_Node*>::iterator it2;
        for (it2 = m_pElements->begin(); it2 != m_pElements->end(); it2++) {
            CElementType_Node * aux = (*it2);
            printf("Switch: %d, Name: %s, Type: %s, Ports:%d, Category: %d, Dimension: %d, Length: %d\n",
                    aux->m_iIdentifier, aux->m_sName, aux->m_sType, (int)aux->m_iPorts, (int)aux->m_iCategory, (int)aux->m_iDimension, (int)aux->m_iLength);
        }

        printf(" Network Channels: %lu\n", m_pChannels->size());

        std::list<CChannelType_Node*>::iterator it3;
        for (it3 = m_pChannels->begin(); it3 != m_pChannels->end(); it3++) {
            CChannelType_Node * aux = (*it3);
            printf("Channel id: %d - Type: %s, DownElement: %d (%s), DownPort: %d (%s), UpElement: %d (%s), UpPort: %d (%s)\n",
                    aux->m_iIdentifier, aux->m_sType, aux->m_iDownElement, aux->m_sDownElement, aux->m_iDownPort, aux->m_sDownPort, aux->m_iUpElement, aux->m_sUpElement, aux->m_iUpPort, aux->m_sUpPort);
        }
    }

#ifndef TOPGEN_PBGL
    
    void topology::build_bgl() {
        // Note: Both IAs & Switches(aka elements) start at 0 inside the inner lists.
        //       Since the vertices of the graph cover all of them, it is required
        //       to shift the switch identifiers: offset = last IA identifier.
        const size_t num_vertices = m_pInputAdapters->size() + m_pElements->size();
        const size_t offset = m_pInputAdapters->size();

        //Populate Graph     
        Graph g(num_vertices);
        
        //printf("Network nodes: %d\n", m_iInputAdapters);

        std::list<CInputAdapterType_Node*>::iterator it1;
        for (it1 = m_pInputAdapters->begin(); it1 != m_pInputAdapters->end(); it1++) {
            CInputAdapterType_Node * aux = (*it1);
            printf("HCA: %d, Name: %s, Type: %s, Value:%s\n", aux->m_iID, aux->m_sName, aux->m_sType, aux->m_sValue);
            g[aux->m_iID].name = aux->m_sName;
            g[aux->m_iID].label = aux->m_sName;
            g[aux->m_iID].id = aux->m_iID;
            g[aux->m_iID].shape = "circle";
            g[aux->m_iID].color = "blue";

        }
        
        //printf("Network Switches: %d\n", m_iSwitchIdentifiers);
        // SW 0 starts at the position of the last IA 
        std::list<CElementType_Node*>::iterator it2;
        for (it2 = m_pElements->begin(); it2 != m_pElements->end(); it2++) {
            CElementType_Node * aux = (*it2);
            printf("Switch: %d, Name: %s, Type: %s, Ports:%d, Category: %c, Dimension: %c, Length: %c\n", aux->m_iIdentifier, aux->m_sName, aux->m_sType, aux->m_iPorts, aux->m_iCategory, aux->m_iDimension, aux->m_iLength);
            g[offset+aux->m_iIdentifier].name = aux->m_sName;
            g[offset+aux->m_iIdentifier].label = aux->m_sName;
            g[offset+aux->m_iIdentifier].id = offset + aux->m_iIdentifier;
            g[offset+aux->m_iIdentifier].shape = "polygon";
            g[offset+aux->m_iIdentifier].color = "red";
        }
        
        //printf(" Network Channels: %d\n", m_iChannels);
        
        std::list<CChannelType_Node*>::iterator it3;
        for (it3 = m_pChannels->begin(); it3 != m_pChannels->end(); it3++) {
            CChannelType_Node * aux = (*it3);
            //printf("Channel id: %d - Type: %s, DownElement: %d (%s), DownPort: %d (%s), UpElement: %d (%s), UpPort: %d (%s)\n",aux->m_iIdentifier, aux->m_sType, aux->m_iDownElement, aux->m_sDownElement, aux->m_iDownPort, aux->m_sDownPort, aux->m_iUpElement, aux->m_sUpElement, aux->m_iUpPort, aux->m_sUpPort);
            // Note: filter according to the name of the element
            //       We assume the down element is always a switch, whereas
            //       the up element can be switch or adapter
            int up_offset = -1;
            int down_offset = -1;
            char tmp[CChannelType_Node::RANGE_MAX];
            // standard channels
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "ia%d", aux->m_iUpElement);
            if (!strncmp(tmp, aux->m_sUpElement, CChannelType_Node::RANGE_MAX)) {
                up_offset = aux->m_iUpElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "s%d", aux->m_iUpElement);
            if (!strncmp(tmp, aux->m_sUpElement, CChannelType_Node::RANGE_MAX)) {
                up_offset = offset + aux->m_iUpElement;;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "ia%d", aux->m_iDownElement);
            if (!strncmp(tmp, aux->m_sDownElement, CChannelType_Node::RANGE_MAX)) {
                down_offset = aux->m_iDownElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "s%d", aux->m_iDownElement);
            if (!strncmp(tmp, aux->m_sDownElement, CChannelType_Node::RANGE_MAX)) {
                down_offset = offset + aux->m_iDownElement;
            }
            // particular channels of KNS
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "ia id %d", aux->m_iUpElement);
            if (!strncmp(tmp, aux->m_sUpElement, CChannelType_Node::RANGE_MAX)) {
                up_offset = aux->m_iUpElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "ia id %d", aux->m_iDownElement);
            if (!strncmp(tmp, aux->m_sDownElement, CChannelType_Node::RANGE_MAX)) {
                down_offset = aux->m_iDownElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "router id %d", aux->m_iUpElement);
            if (!strncmp(tmp, aux->m_sUpElement, CChannelType_Node::RANGE_MAX)) {
                up_offset = offset + aux->m_iUpElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "router id %d", aux->m_iDownElement);
            if (!strncmp(tmp, aux->m_sDownElement, CChannelType_Node::RANGE_MAX)) {
                down_offset = offset + aux->m_iDownElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "switch id %d", aux->m_iUpElement);
            if (!strncmp(tmp, aux->m_sUpElement, CChannelType_Node::RANGE_MAX)) {
                up_offset = offset + aux->m_iUpElement;
            }
            snprintf(tmp, CChannelType_Node::RANGE_MAX, "switch id %d", aux->m_iDownElement);
            if (!strncmp(tmp, aux->m_sDownElement, CChannelType_Node::RANGE_MAX)) {
                down_offset = offset + aux->m_iDownElement;
            }
            assert(up_offset != -1);
            assert(down_offset != -1);
            //edge builder duplicates the name into name and label
            boost::add_edge(down_offset, up_offset, {aux->m_sName, (int)offset + aux->m_iIdentifier, rand() / double(RAND_MAX)}, g);            
        }
        
        m_Graph = g;
    }
    
    void topology::write_graphviz(const std::string gvfile)
    {
        std::ofstream outf(gvfile);
        
        // First way. Basic without properties.
        //boost::write_graphviz(outf, m_Graph,
        //        make_label_writer(get(&VertexData::name, m_Graph)),
        //        make_label_writer(get(&EdgeData::name, m_Graph)));
       
        // Second way. Advanced with properties
        boost::dynamic_properties dp;
        //the property 'node_id' is important and compulsory to generate the .dot
        //dp.property("node_id", get(&VertexData::id, m_Graph)); 
        dp.property("node_id", get(&VertexData::name, m_Graph)); 
        dp.property("label", get(&VertexData::label, m_Graph));
        dp.property("shape", get(&VertexData::shape, m_Graph));
        dp.property("color", get(&VertexData::color, m_Graph));
        dp.property("label", get(&EdgeData::label, m_Graph));
        dp.property("weight", get(&EdgeData::weight, m_Graph));
          
        boost::write_graphviz_dp(outf, m_Graph, dp);
    }
    
    void topology::run_read_graphviz(const std::string gvfile)
    {
        Graph g; //g is temporal
        std::ifstream inf(gvfile);
        boost::dynamic_properties dp;
        dp.property("node_id", get(&VertexData::name, g)); 
        dp.property("label", get(&VertexData::label, g));
        dp.property("shape", get(&VertexData::shape, g));
        dp.property("color", get(&VertexData::color, g));
        dp.property("label", get(&EdgeData::label, g));
        dp.property("weight", get(&EdgeData::weight, g));
        bool status = boost::read_graphviz(inf, g, dp/*, "node_id"*/);
        if (!status)
            throw new CException("run_read_graphviz(): cannot read from file");
        
        // Uncomment to compare
        std::ofstream outf("bgl-compare.gv");
        boost::write_graphviz_dp(outf, g, dp);
    }
   
    void topology::write_graphml(const std::string mlfile)
    {
        std::ofstream outf(mlfile);
        
        // First way. Basic without properties.
        //boost::write_graphviz(outf, m_Graph,
        //        make_label_writer(get(&VertexData::name, m_Graph)),
        //        make_label_writer(get(&EdgeData::name, m_Graph)));
       
        // Second way. Advanced with properties
        boost::dynamic_properties dp;
        //the property 'node_id' is important and compulsory to generate the .dot
        //dp.property("node_id", get(&VertexData::id, m_Graph)); 
        dp.property("node_id", get(&VertexData::name, m_Graph)); 
        dp.property("label", get(&VertexData::label, m_Graph));
        dp.property("shape", get(&VertexData::shape, m_Graph));
        dp.property("color", get(&VertexData::color, m_Graph));
        dp.property("weight", get(&EdgeData::weight, m_Graph));
          
        boost::write_graphml(outf, m_Graph, dp);
    }
    
    void topology::print_graph()
    {
        // Print out some useful information on standard output
        std::cout << "Graph:" << std::endl;
        boost::print_graph(m_Graph, boost::get(&VertexData::name, m_Graph));
        std::cout << "num_vertexes: " << boost::num_vertices(m_Graph) << std::endl;
        std::cout << "num_edges: " << boost::num_edges(m_Graph) << std::endl;
    }
    
    void topology::run_dijkstra_shortest_paths(int from, int to)
    {
        Vertex_Descriptor s = boost::vertex(from, m_Graph); // indicate the index of the vextex
        Vertex_Descriptor t = boost::vertex(to, m_Graph);   // idem
      
        const int num_vertixes = boost::num_vertices(m_Graph);
        
        // Define weights
        boost::property_map<Graph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, m_Graph);
        
        // Define distances
        std::vector<int> distances(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            distances[v] = 1; // TODO allow users to preset this
        }
        auto distances_ipmap = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, m_Graph));

        // Define predecessors
        std::vector<Vertex_Descriptor> predecessors(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            distances[v] = v;
        }
        auto predecessors_ipmap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, m_Graph));
        
        // Run algorithm
        boost::dijkstra_shortest_paths(m_Graph, s,
            boost::weight_map(weight_pmap).distance_map(distances_ipmap).predecessor_map(predecessors_ipmap));
        
        // Extract the shortest path from s to t.
        typedef std::vector<Edge_Descriptor> path_t;
        path_t path;

        Vertex_Descriptor v = t;
        for(Vertex_Descriptor u = predecessors[v]; u != v; v=u, u=predecessors[v])
        {
          std::pair<Edge_Descriptor, bool> edge_pair = boost::edge(u,v,m_Graph);
          path.push_back( edge_pair.first );
        }

        std::cout << std::endl;
        std::cout << "Dijsktra's shortest path from s to t:" << std::endl;
        for(path_t::reverse_iterator riter = path.rbegin(); riter != path.rend(); ++riter)
        {
          Vertex_Descriptor u_tmp = boost::source(*riter, m_Graph);
          Vertex_Descriptor v_tmp = boost::target(*riter, m_Graph);
          Edge_Descriptor   e_tmp = boost::edge(u_tmp, v_tmp, m_Graph).first;

          std::cout << "  " << m_Graph[u_tmp].name << " -> " << m_Graph[v_tmp].name << "    (weight: " << m_Graph[e_tmp].weight << ")" << std::endl;
        }

        std::cout << std::endl;
    }
    
    void topology::run_bellman_ford_shortest_paths(int from, int to)
    {
        Vertex_Descriptor s = boost::vertex(from, m_Graph); // indicate the index of the vextex
        Vertex_Descriptor t = boost::vertex(to, m_Graph);   // idem 
        
        const int num_vertixes = boost::num_vertices(m_Graph);
        //bool r = boost::bellman_ford_shortest_paths(m_Graph, N,
        //    boost::weight_map(boost::get(&EdgeData::weight,m_Graph))
        //    .distance_map(boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index,m_Graph)))
        //    .predecessor_map(boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,m_Graph)))
        //    );
        
        // Define weights map
        //boost::property_map<Graph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, m_Graph);
        boost::property_map<Graph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, m_Graph);
        
        // Define distances
        std::vector<int> distances(num_vertixes, (std::numeric_limits<int>::max)());
        //for (int v=0; v<num_vertixes; ++v) {
        //    distances[v] = 1; // TODO allow users to preset this
        //}
        distances[from] = 0; // the source is at distance 0
        auto distance_ipmap = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, m_Graph));
        
        // Define predecessors
        std::vector<Vertex_Descriptor> predecessors(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            predecessors[v] = v;
        }
        auto predecessor_ipmap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, m_Graph));
        
        // Run algorithm
        //bool r = boost::bellman_ford_shortest_paths(m_Graph, s,
        //    boost::weight_map(weight_pmap).distance_map(distance_ipmap).predecessor_map(predecessor_ipmap));

        bool r = boost::bellman_ford_shortest_paths(m_Graph, num_vertixes,
            boost::weight_map(weight_pmap).distance_map(&distances[0]).predecessor_map(&predecessors[0]));

        if (r) {            
            // The resulting solution of the algorithm is a table. So we print the table.       
            // Extract the shortest path from s to t.
            typedef std::vector<Edge_Descriptor> path_t;
            path_t path;

            Vertex_Descriptor v = t;
            for(Vertex_Descriptor u = predecessors[v]; u != v; v=u, u=predecessors[v])
            {
              std::pair<Edge_Descriptor, bool> edge_pair = boost::edge(u,v,m_Graph);
              path.push_back( edge_pair.first );
            }

            std::cout << std::endl;
            std::cout << "Bellman-Ford's shortest path from s to t:" << std::endl;
            for(path_t::reverse_iterator riter = path.rbegin(); riter != path.rend(); ++riter)
            {
              Vertex_Descriptor u_tmp = boost::source(*riter, m_Graph);
              Vertex_Descriptor v_tmp = boost::target(*riter, m_Graph);
              Edge_Descriptor   e_tmp = boost::edge(u_tmp, v_tmp, m_Graph).first;

              std::cout << "  " << m_Graph[u_tmp].name << " -> " << m_Graph[v_tmp].name << "    (weight: " << m_Graph[e_tmp].weight << ")" << std::endl;
            }

            std::cout << std::endl;
        } else {
            std::cout << "negative cycle" << std::endl;
        }
    }
    
    void topology::run_breadth_first_search(int from)
    {
        Vertex_Descriptor s = boost::vertex(from, m_Graph); // indicate the index of the vextex
        
        const int num_vertixes = boost::num_vertices(m_Graph);
        
        // Typedefs
        typedef boost::graph_traits<Graph>::vertices_size_type size_type;

        // a vector to hold the discover time property for each vertex
        std::vector<size_type> dtime(num_vertixes);
        typedef boost::iterator_property_map<std::vector<size_type>::iterator, 
                boost::property_map<Graph, boost::vertex_index_t>::const_type> dtime_pm_type;
        dtime_pm_type dtime_pm(dtime.begin(), boost::get(boost::vertex_index, m_Graph));

        size_type time = 0;
        bfs_time_visitor < dtime_pm_type >vis(dtime_pm, time);
        boost::breadth_first_search(m_Graph, boost::vertex(s, m_Graph), visitor(vis));

        // Use std::sort to order the vertices by their discover time
        std::vector<boost::graph_traits<Graph>::vertices_size_type> discover_order(num_vertixes);
        boost::integer_range<int> range(0, num_vertixes);
        std::copy(range.begin(), range.end(), discover_order.begin());
        std::sort(discover_order.begin(),
                  discover_order.end(),
                  boost::indirect_cmp<dtime_pm_type, std::less<size_type> >(dtime_pm));

        std::cout << "Breadth First Search. Order of discovery: " << std::endl;
        for (int i = 0; i < num_vertixes; ++i) {
            Vertex_Descriptor v_tmp = boost::vertex(discover_order[i], m_Graph);
            std::cout << "   " << i << " : " << m_Graph[v_tmp].name << std::endl;
        }
        std::cout << std::endl;
    }
    
    void topology::run_depth_first_search(int from)
    {
        Vertex_Descriptor s = boost::vertex(from, m_Graph); // indicate the index of the vextex
        
        const int num_vertixes = boost::num_vertices(m_Graph);
        
        // Typedefs
        typedef boost::graph_traits<Graph>::vertices_size_type size_type;
        
        // discover time and finish time properties
        std::vector<size_type> dtime(num_vertixes);
        std::vector<size_type> ftime(num_vertixes);
        typedef boost::iterator_property_map<std::vector<size_type>::iterator,
                boost::property_map<Graph, boost::vertex_index_t>::const_type>
                time_pm_type;
        
        time_pm_type dtime_pm(dtime.begin(), boost::get(boost::vertex_index, m_Graph));
        time_pm_type ftime_pm(ftime.begin(), boost::get(boost::vertex_index, m_Graph));
        
        size_type t = 0;
        dfs_time_visitor<time_pm_type> vis(dtime_pm, ftime_pm, t);

        boost::depth_first_search(m_Graph, visitor(vis));  //what about s?

        // use std::sort to order the vertices by their discover time
        std::vector<size_type> discover_order(num_vertixes);
        boost::integer_range<size_type> range(0, num_vertixes);
        std::copy(range.begin(), range.end(), discover_order.begin());
        std::sort(discover_order.begin(),
                  discover_order.end(),
                  boost::indirect_cmp<time_pm_type, std::less<size_type> >(dtime_pm));
        
        // later, use std::sort to order the vertices by their finish time
        std::vector<size_type> finish_order(num_vertixes);
        std::copy(range.begin(), range.end(), finish_order.begin());
        std::sort(finish_order.begin(),
                  finish_order.end(),
                  boost::indirect_cmp<time_pm_type, std::less<size_type> >(ftime_pm));
        
        std::cout << "Depth First Search. Orders of discovery + finish: " << std::endl;
        for (int i = 0; i < num_vertixes; ++i) {
            Vertex_Descriptor d_tmp = boost::vertex(discover_order[i], m_Graph);
            Vertex_Descriptor f_tmp = boost::vertex(finish_order[i], m_Graph);
            std::cout << "  " << i << " : " << m_Graph[d_tmp].name; 
            std::cout << " : " << m_Graph[f_tmp].name << std::endl;
        }
        std::cout << std::endl;
    }
         
    void topology::run_kruskal_minimum_spanning_tree(int root_tree)
    {
        const int num_vertixes = boost::num_vertices(m_Graph);
        //-std::vector<Vertex_Descriptor> predecessors(boost::num_vertices(m_Graph));
        //-std::vector<int> distances(boost::num_vertices(m_Graph));
        std::vector<Edge_Descriptor> spanning_tree;
        //-auto weightmap = get(&EdgeData::weight, m_Graph);
        ///
        //-boost::get(&EdgeData::weight, m_Graph)
        //-boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index,m_Graph))
        //boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,m_Graph))
        ///
        ///
        boost::property_map<Graph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, m_Graph);
        
        // Define distances
        std::vector<int> distances(num_vertixes, (std::numeric_limits<int>::max)());
        //for (int v=0; v<num_vertixes; ++v) {
        //    distances[v] = 1; // TODO allow users to preset this
        //}
        distances[root_tree] = 0; // the root of the spanning tree is at distance 0
        auto distance_ipmap = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, m_Graph));
        
        // Define the spanning tree consisting in predecessors
        std::vector<Vertex_Descriptor> predecessors(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            predecessors[v] = v;
        }
        auto predecessor_ipmap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, m_Graph));
        
        // named parameter version
        kruskal_minimum_spanning_tree(m_Graph, std::back_inserter(spanning_tree), 
            boost::weight_map(weight_pmap)
            .distance_map(distance_ipmap)
            .predecessor_map(predecessor_ipmap)
            );
        
        // Determine the weight of the minimum spanning tree
        std::cout<<"Kruskal's minimum spanning tree:" << std::endl;
        double w = 0;
        edge_iter ei, ei_end;
        for (tie(ei, ei_end) = edges(m_Graph); ei != ei_end; ++ei) {
            w += weight_pmap[*ei];

            Vertex_Descriptor u_tmp = boost::source(*ei, m_Graph);
            Vertex_Descriptor v_tmp = boost::target(*ei, m_Graph);
            Edge_Descriptor   e_tmp = boost::edge(u_tmp, v_tmp, m_Graph).first;
            std::cout << "  " << m_Graph[u_tmp].name;
            std::cout << " -" << m_Graph[e_tmp].name << "-> ";
            std::cout << m_Graph[v_tmp].name;
            std::cout << "  (weight: " << m_Graph[e_tmp].weight << ")" << std::endl;
        }
        
        std::cout<<"  The weight is " << w << std::endl;
    }
    
    //https://www.boost.org/doc/libs/1_69_0/libs/graph/example/prim-example.cpp
    void topology::run_prim_minimum_spanning_tree(int root_tree) 
    {
        const int num_vertixes = boost::num_vertices(m_Graph);
        
        //Vertex_Descriptor s = boost::vertex(from, m_Graph); // indicate the index of the vextex
        
        //auto distance = get(boost::vertex_distance, m_Graph);
        //-auto weightmap = get(&EdgeData::weight, m_Graph);
        //auto indexmap = get(boost::vertex_index, m_Graph);
        
        boost::property_map<Graph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, m_Graph);
        
        // Define distances
        std::vector<int> distances(num_vertixes, (std::numeric_limits<int>::max)());
        //for (int v=0; v<num_vertixes; ++v) {
        //    distances[v] = 1; // TODO allow users to preset this
        //}
        distances[root_tree] = 0; // the root of the spanning tree is at distance 0
        auto distance_ipmap = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, m_Graph));
        
        // Define the spanning tree consisting in predecessors
        std::vector<Vertex_Descriptor> predecessors(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            predecessors[v] = v;
        }
        auto predecessor_ipmap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, m_Graph));
        
        boost::property_map<Graph, boost::vertex_index_t>::type index_pmap = get(boost::vertex_index, m_Graph);
        
        // named parameter version
        boost::prim_minimum_spanning_tree(m_Graph,
                &predecessors[0],
                boost::predecessor_map(predecessor_ipmap)
                .distance_map(distance_ipmap)
                .weight_map(weight_pmap));
        
        // Determine the weight of the minimum spanning tree
        std::cout<<"Prim's minimum spanning tree:" << std::endl;
        double w = 0;
        edge_iter ei, ei_end;
        for (tie(ei, ei_end) = edges(m_Graph); ei != ei_end; ++ei) {
            w += weight_pmap[*ei];

            Vertex_Descriptor u_tmp = boost::source(*ei, m_Graph);
            Vertex_Descriptor v_tmp = boost::target(*ei, m_Graph);
            Edge_Descriptor   e_tmp = boost::edge(u_tmp, v_tmp, m_Graph).first;
            std::cout << "  " << m_Graph[u_tmp].name;
            std::cout << " -" << m_Graph[e_tmp].name << "-> ";
            std::cout << m_Graph[v_tmp].name;
            std::cout << "  (weight: " << m_Graph[e_tmp].weight << ")" << std::endl;
        }
        
        std::cout <<"  The weight is " << w << std::endl;
    }
#endif //TOPGEN_PBGL
    
} // namespace topology
