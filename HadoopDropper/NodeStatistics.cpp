/*
 * NodeStatistics.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Derek Weitzel
 */

#include "NodeStatistics.h"
#include <list>
#include <ctime>

using namespace std;

NodeStatistics::NodeStatistics(int timeout)
{
	// TODO Auto-generated constructor stub

	this->SetTimeout(timeout);


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

}

double NodeStatistics::GetLoad()
{
	list<event_time>::iterator it = this->m_events.begin();
	double load_sum = 0;
	time_t now = time(NULL);
	time_t del_thres = now - this->m_timeout;

	// Delete the expired values
	while(it->time < del_thres)
	{
		it = this->m_events.erase(it);
	}

	// Get the loads, a linear function
	for ( it = this->m_events.begin(); it != this->m_events.end(); it++)
	{
		load_sum += it->load * ( this->m_timeout - ( now - it->time ) );
	}

	return load_sum;

}


