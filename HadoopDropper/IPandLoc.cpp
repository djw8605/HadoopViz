/*
 * IPandLoc.cpp
 *
 *  Created on: Jun 17, 2009
 *      Author: dweitzel
 */

#include <cstring>
#include <cstdio>
#include <cmath>

#include "IPandLoc.h"
#define EXP_DECAY 0.985
#define MAX_LOAD 300.0

IPandLoc::IPandLoc()
{
    // TODO Auto-generated constructor stub
    m_ip = 0;
    this->m_load = 0.0;
}

IPandLoc::~IPandLoc()
{
    // TODO Auto-generated destructor stub
    if(m_ip)
        delete m_ip;

}

IPandLoc::IPandLoc(char* ip)
{
    m_ip = new char[strlen(ip)+1];
    strcpy(m_ip, ip);
    this->m_load = 0.0;

}

IPandLoc::IPandLoc(const IPandLoc& rhs)
{
            m_ip = new char[strlen(rhs.m_ip) + 1];
            strcpy(m_ip, rhs.m_ip);

            m_point = rhs.m_point;
            m_load = rhs.m_load;

}


IPandLoc::IPandLoc(char* ip, point p)
{
    m_ip = new char[strlen(ip)+1];
    strcpy(m_ip, ip);

    m_point = p;
    this->m_load = 0.0;


}


bool IPandLoc::operator==(const IPandLoc &other) const
{

    if(strcmp(other.m_ip, m_ip) == 0)
        return true;
    else
        return false;

}


bool operator < (IPandLoc &lhs, IPandLoc &rhs)
{

    if(strcmp(lhs.m_ip, rhs.m_ip) < 0)
        return true;
    else
        return false;


}

IPandLoc & IPandLoc::operator=(const IPandLoc &rhs)
{

    if ( this != &rhs)
    {
        if(m_ip)
            delete [] m_ip;
        m_ip = new char[strlen(rhs.m_ip) + 1];
        strcpy(m_ip, rhs.m_ip);

        m_point = rhs.m_point;
        m_load = rhs.m_load;

    }
    return *this;


}

void IPandLoc::ExpDecayLoad()
{
	this->m_load *= EXP_DECAY;


}

void IPandLoc::AddLoad(float amount)
{

	this->m_load = fmin(MAX_LOAD, m_load+3*amount);

	//if(this->m_load > 500)
		//printf("%lf += %lf\n", this->m_load, amount);
	//printf("adding %lf\n", amount);
}


float IPandLoc::GetMaxLoad()
{

	return MAX_LOAD;

}


