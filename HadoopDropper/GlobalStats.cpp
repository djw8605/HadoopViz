/*
 * GlobalStats.cpp
 *
 *  Created on: Jun 14, 2010
 *      Author: derekweitzel
 */

#include "GlobalStats.h"

GlobalStats::GlobalStats()
{
	// TODO Auto-generated constructor stub

}

GlobalStats::~GlobalStats()
{
	// TODO Auto-generated destructor stub
}

GlobalStats* GlobalStats::GetInstance()
{
	static GlobalStats* inst = new GlobalStats();
	return inst;
}


double GlobalStats::GetMaxLoad()
{
	return this->m_maxload;

}

void GlobalStats::SetLoad(double load)
{
	if ( load > this->m_maxload)
		this->m_maxload = load;


}

