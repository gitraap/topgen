/*
 *  Exception.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include "topgen/exception.hpp"

namespace topgen {

    CException::CException(const char * message) {
        m_sMessage = message;
        fprintf(stderr, "CException thrown with message \"%s\"\n", message);
    }

    CException::~CException() {

    }

    const char * CException::Report() {
        return m_sMessage;
    }

} // namespace topology
