/*
 * GlobalStats.h
 *
 *  Created on: Jun 14, 2010
 *      Author: derekweitzel
 */

#ifndef GLOBALSTATS_H_
#define GLOBALSTATS_H_

#define _stats GlobalStats::GetInstance()

class GlobalStats
{
public:
	GlobalStats();
	virtual ~GlobalStats();
	static GlobalStats* GetInstance();

	double GetMaxLoad();
	void SetLoad(double load);

private:
	double m_maxload;


};

#endif /* GLOBALSTATS_H_ */
