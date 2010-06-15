/*
 * IPandLoc.h
 *
 *  Created on: Jun 17, 2009
 *      Author: dweitzel
 */

#ifndef IPANDLOC_H_
#define IPANDLOC_H_

#include "Math/Math.h"
#include "HadoopDropper/NodeStatistics.h"

class IPandLoc
{
public:
    IPandLoc();
    IPandLoc(char* ip);
    IPandLoc(char* ip, point p);
    IPandLoc(const IPandLoc& rhs);
    virtual ~IPandLoc();

    IPandLoc & operator=(const IPandLoc &rhs);
    //IPandLoc & operator=(const char* rhs);

    friend bool operator < (const IPandLoc &lhs, const IPandLoc &rhs);
    //friend bool operator < (IPandLoc &lhs, char &rhs);
    //friend bool operator < (char &lhs, IPandLoc &rhs);

    bool operator==(const IPandLoc &other) const;
    bool operator==(const char *other) const;

    point GetPoint() { return m_point; };
    void SetPoint(point p);
    char* GetIP() { return m_ip; };

    float GetLoad();
    void AddLoad(float amount);
    void ExpDecayLoad();


private:
    point m_point;
    char* m_ip;
    float m_load;
    NodeStatistics m_stats;


};

#endif /* IPANDLOC_H_ */
