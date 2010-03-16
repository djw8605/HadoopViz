/*
 * IPandLoc.h
 *
 *  Created on: Jun 17, 2009
 *      Author: dweitzel
 */

#ifndef IPANDLOC_H_
#define IPANDLOC_H_

#include "Math/Math.h"

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

    friend bool operator < (IPandLoc &lhs, IPandLoc &rhs);
    //friend bool operator < (IPandLoc &lhs, char &rhs);
    //friend bool operator < (char &lhs, IPandLoc &rhs);

    bool operator==(const IPandLoc &other) const;

    point GetPoint() { return m_point; };
    char* GetIP() { return m_ip; };

    float GetLoad() { return m_load; };
    void AddLoad(float amount);
    float GetMaxLoad();
    void ExpDecayLoad();

    point GetPos() { return m_point; };


private:
    point m_point;
    char* m_ip;
    float m_load;


};

#endif /* IPANDLOC_H_ */
