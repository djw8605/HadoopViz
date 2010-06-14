/*
 * GlobalStats.cpp
 *
 *  Created on: Jun 14, 2010
 *      Author: derekweitzel
 */

#include "GlobalStats.h"
#include "IPtoLocation.h"

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
	// Do nothing yet

}


void GlobalStats::UpdateStats()
{
	// Get the max current load
	this->m_maxload = 0.0;
	for (int i = 0; i < _iploc->GetSize(); i++)
	{
		if (_iploc->GetLoad(i) > this->m_maxload)
			this->m_maxload = _iploc->GetLoad(i);
	}




}


