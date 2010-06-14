/*
 * NodeStatistics.h
 *
 *  Created on: Jun 13, 2010
 *      Author: derekweitzel
 */

#ifndef NODESTATISTICS_H_
#define NODESTATISTICS_H_

#include <list>

struct event_time
{
	double load;
	long int time;
};


class NodeStatistics
{
public:
	NodeStatistics(int timeout);
	virtual ~NodeStatistics();

	void AddEvent(double load);
	double GetLoad();
	void SetTimeout(int time);
	void UpdateEvents();

private:
	std::list<event_time> m_events;
	int m_timeout;

};

#endif /* NODESTATISTICS_H_ */
