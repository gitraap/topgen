/*
 *  SCPManager.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_SCP_MANAGER_HPP
#define TOPGEN_SCP_MANAGER_HPP

#include <stdio.h>

namespace topgen {

class CSCPManager {
public:
	CSCPManager(const char * pattern_file_name, int stages, int switches_stage, int clusters, int radix);
	virtual ~CSCPManager();

	int GetCluster(int st, int sw, int port);

private:
	const char * m_sPatternFileName;
	FILE * m_pPatternFile;
	int m_iStages;
	int m_iSwitchesPerStage;
	int m_iClusters;
	int m_iSwitchRadix;
	int m_iPortsPerCluster;

	int *** m_pCluster; // stage x switch x port -> cluster_id
};

}

#endif /* TOPGEN_SCP_MANAGER_HPP */
