/*
 * IptoLocation.cpp
 *
 *  Created on: Jun 17, 2009
 *      Author: dweitzel
 */

#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstring>

#include "Math/Math.h"
#include "IPtoLocation.h"
#include "config.h"

float modified_colors[3] = { 1.0, 0.0, 0.0};

/* Note to self
 * arc equation: (-(((2/dist)*x - 1)^2) + 1) * sacle
 *
 *
 */

using namespace std;

extern vector<char*> modified_ips;

IPtoLocation::IPtoLocation()
{
    // TODO Auto-generated constructor stub


}

IPtoLocation::~IPtoLocation()
{
    // TODO Auto-generated destructor stub
    ips.clear();
}


IPtoLocation* IPtoLocation::GetInstance()
{

    static IPtoLocation* inst = new IPtoLocation();
    return inst;

}

point IPtoLocation::GetLocation(char* ip)
{

    IPandLoc iploc(ip);
    std::vector<IPandLoc>::iterator i;
    i = find(ips.begin(), ips.end(), iploc);
    //printf("after find\n");
    if (i == ips.end())
    {

        AddIP(ip);
        //printf("adding ip\n");
        return ips.back().GetPoint();

    } else
        return (*i).GetPoint();




}


void IPtoLocation::AddIP(char* ip)
{
//    static int counter = 0;
    /* initialize the point */
    point p;
    //p.x = (counter / AREA_SIZE) * SPACE_BETWEEN;
    //p.y = (counter % AREA_SIZE);
    //p.z = 0;
    p.x = (rand() % 100) - 50;
    p.y = (rand() % 100) - 50;
    p.z = (rand() % 100) - 50;
    IPandLoc iploc(ip, p);
    for(unsigned int i = 0; i < modified_ips.size(); i++)
    {
    	if (strcmp(modified_ips[i], ip) == 0)
    	{
    		iploc.SetColor(modified_colors);
    		//printf("Found: %s", ip);
    		break;

    	}
    }

    ips.push_back(iploc);
    //printf("%lf, %lf, %i\n", p.x, p.y, counter);

//    counter+=SPACE_BETWEEN;
}


char* IPtoLocation::GetIP(unsigned int position)
{
    if((position > ips.size()) || (position < 0))
        return 0;
    else
        return ips[position].GetIP();



}

point IPtoLocation::GetLocation(unsigned int position)
{
    point p;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    if((position > ips.size()) || (position < 0))
        return p;
    else
        return ips[position].GetPoint();

}


void IPtoLocation::ExpDecayLoads()
{
	std::vector<IPandLoc>::iterator i;
	for(i = ips.begin(); i != ips.end(); i++)
	{
		(*i).ExpDecayLoad();
		/*if((*i).GetLoad() > 0.0)
			printf("%s: Load = %lf\n", (*i).GetIP(), (*i).GetLoad());
*/
	}

}

void IPtoLocation::RemoveNode(unsigned int position)
{
	std::vector<IPandLoc>::iterator i = ips.begin();

	ips.erase(i+position);

}

IPandLoc IPtoLocation::GetInfo(unsigned int position)
{
	return ips[position];


}

float IPtoLocation::GetLoad(unsigned int position)
{
	return ips[position].GetLoad();


}

IPandLoc& IPtoLocation::GetByIP(char* ip)
{

	IPandLoc iploc(ip);
	    std::vector<IPandLoc>::iterator i;
	    i = find(ips.begin(), ips.end(), iploc);
	    //printf("after find\n");
	    if (i == ips.end())
	    {

	        AddIP(ip);
	        //printf("adding ip\n");
	        return ips.back();

	    } else
	        return (*i);


}

IPandLoc& IPtoLocation::GetByIP(char* ip, float load_mod)
{
	IPandLoc iploc(ip);
		    std::vector<IPandLoc>::iterator i;
		    i = find(ips.begin(), ips.end(), iploc);
		    //printf("after find\n");
		    if (i == ips.end())
		    {

		        AddIP(ip);
		        //printf("adding ip\n");
		        ips.back().AddLoad(load_mod);
		        return ips.back();

		    } else
		    {
		    	(*i).AddLoad(load_mod);
		        return (*i);
		    }


}




