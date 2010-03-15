/*
 * IptoLocation.h
 *
 *  Created on: Jun 17, 2009
 *      Author: dweitzel
 */

#ifndef IPTOLOCATION_H_
#define IPTOLOCATION_H_

#include <vector>

#include "Math/Math.h"
#include "HadoopDropper/IPandLoc.h"
#define _iploc IPtoLocation::GetInstance()

class IPtoLocation
{
public:
    IPtoLocation();
    virtual ~IPtoLocation();
    static IPtoLocation* GetInstance();

    point GetLocation(char* ip);
    point GetLocation(unsigned int position);

    int GetSize() { return ips.size(); };

    char* GetIP(unsigned int position);
    void ExpDecayLoads();
    IPandLoc GetInfo(unsigned int position);
    float GetLoad(unsigned int position);
    void RemoveNode(unsigned int position);

    IPandLoc& GetByIP(char* ip);
    IPandLoc& GetByIP(char* ip, float load_mod);

private:

    void AddIP(char* ip);

    std::vector<IPandLoc> ips;

};

#endif /* IPTOLOCATION_H_ */
