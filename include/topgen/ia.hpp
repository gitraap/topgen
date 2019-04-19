/*
 *  IA.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_IA_HPP
#define TOPGEN_IA_HPP

namespace topgen {

class CIA {
public:
	CIA(int id);
	virtual ~CIA();

	int identifier;
};

}

#endif /* TOPGEN_IA_HPP */
