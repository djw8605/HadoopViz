/*
 * NodeStatistics.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Derek Weitzel
 */

#include "NodeStatistics.h"
#include <vector>
#include <ctime>

using namespace std;

NodeStatistics::NodeStatistics(int timeout)
{
	// TODO Auto-generated constructor stub

	this->SetTimeout(timeout);
	this->m_recalcload = true;
	printf("Inside create\n");


}

NodeStatistics::NodeStatistics(const NodeStatistics& ns)
{
	this->m_events = ns.m_events;
	this->m_lastload = ns.m_lastload;
	this->m_recalcload = ns.m_recalcload;
	this->m_timeout = ns.m_timeout;
	//printf("Inside copy\n");

}


NodeStatistics::~NodeStatistics()
{
	// TODO Auto-generated destructor stub

}



void NodeStatistics::SetTimeout(int time)
{
	this->m_timeout = time;


}

void NodeStatistics::AddEvent(double load)
{
	event_time et;
	et.load = load;
	et.time = time(NULL);
	this->m_events.push_back(et);
	this->m_recalcload = true;

}

double NodeStatistics::GetLoad()
{
	if (this->m_events.size() == 0)
		return 0;

	vector<event_time>::iterator it = this->m_events.begin();
	if (it == this->m_events.end())
		return 0;

	double load_sum = 0;
	time_t now = time(NULL);
	time_t del_thres = now - this->m_timeout;



	// Delete the expired values
	if (this->m_lasttime != now)
	{
		while( (it != this->m_events.end()) && (it->time < del_thres))
		{
			it++;
			this->m_recalcload = true;
		}
	}


	// If you didn't delete any values, and you haven't added anything to the queue,
	// then just return what you did last time (nothing changed)
	if (!this->m_recalcload)
		return this->m_lastload;

	if (it != this->m_events.begin())
	{
		it = this->m_events.erase(this->m_events.begin(), it);
	}




	if (this->m_events.size() == 0)
		return 0;



	// Sums the loads
	for ( it = this->m_events.begin(); it != this->m_events.end(); it++)
	{
		load_sum += it->load; //it->load * ( this->m_timeout - ( now - it->time ) );
	}

	this->m_lastload = (load_sum / (double)this->m_timeout);
	this->m_lasttime = now;
	return this->m_lastload;

}


