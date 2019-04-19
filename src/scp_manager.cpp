/*
 *  SCPManager.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/topology.hpp"
#include "topgen/scp_manager.hpp"
#include "topgen/exception.hpp"

namespace topgen {

CSCPManager::CSCPManager(const char * pattern_file_name, int stages, int switches_stage, int clusters, int radix) {
	m_sPatternFileName = pattern_file_name;
	m_iStages = stages;
	m_iSwitchesPerStage = switches_stage;
	m_iClusters = clusters;
	m_iSwitchRadix = radix;
	m_iPortsPerCluster = radix/clusters;

	m_pPatternFile = fopen(m_sPatternFileName,"r");

	if (!m_pPatternFile)
		throw new CException("CSCPManager: CSCPManager(): File did not open");

	// allocate space
	m_pCluster = new int**[m_iStages];
	for (int a=0; a<m_iStages; a++)
	{
		m_pCluster[a] = new int*[m_iSwitchesPerStage];
		for (int b=0; b<m_iSwitchesPerStage; b++)
		{
			m_pCluster[a][b] = new int[m_iSwitchRadix];
			for (int c=0; c<m_iSwitchRadix; c++)
				m_pCluster[a][b][c] = -1;
		}
	}

	// read from file
	int st;
	int sw;
	int pt;

	while(!feof(m_pPatternFile))
	{
		int read = fscanf( m_pPatternFile, "%d %d", &st, &sw); // new line
		if (read != 2) break;

		int cluster = 0;
		int port_per_cluster = m_iPortsPerCluster;
		for (int i=0; i<m_iSwitchRadix; i++) {
			read = fscanf( m_pPatternFile, "%d", &pt);
			if (read != 1) {
				throw new CException("CSCPManager: CSCPManager(): ");
			}
			port_per_cluster--;
			m_pCluster[st][sw][pt] = cluster;


			if (port_per_cluster == 0) {
				cluster++;
				port_per_cluster = m_iPortsPerCluster;
			}
		}

	}

}

CSCPManager::~CSCPManager() {
	for (int a=0; a<m_iStages; a++)
	{
		for (int b=0; b<m_iSwitchesPerStage; b++)
		{
			delete [] m_pCluster[a][b];
		}
		delete [] m_pCluster[a];
	}
	delete [] m_pCluster;
}

int CSCPManager::GetCluster(int st, int sw, int port)
{
	return m_pCluster[st][sw][port];
}

}
