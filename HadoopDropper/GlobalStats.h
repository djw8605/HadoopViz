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
	double GetSumLoad();
	void SetLoad(double load);
	void UpdateStats();
	const double* GetHostLoads(int * size);



private:
	double m_maxload;
	double m_loadsum;

	double* m_hostloads;
	int m_hostloadsize;


};

#endif /* GLOBALSTATS_H_ */
