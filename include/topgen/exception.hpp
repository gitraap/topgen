/*
 *  Exception.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_EXCEPTION_HPP
#define TOPGEN_EXCEPTION_HPP

namespace topgen {

class CException
{
public:
	CException(const char * message);
	virtual ~CException();

	const char * Report();

private:
	const char * m_sMessage;
};

} // namespace topology

#endif
