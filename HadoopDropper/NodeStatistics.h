/*
 * NodeStatistics.h
 *
 *  Created on: Jun 13, 2010
 *      Author: derekweitzel
 */

#ifndef NODESTATISTICS_H_
#define NODESTATISTICS_H_

#include <vector>

struct event_time
{
	double load;
	long int time;
};


class NodeStatistics
{
public:
	NodeStatistics(int timeout);
	NodeStatistics(const NodeStatistics& ns);
	virtual ~NodeStatistics();

	void AddEvent(double load);
	double GetLoad();
	void SetTimeout(int time);
	void UpdateEvents();

private:
	std::vector<event_time> m_events;
	int m_timeout;
	double m_lastload;
	bool m_recalcload;
	time_t m_lasttime;

};

#endif /* NODESTATISTICS_H_ */
