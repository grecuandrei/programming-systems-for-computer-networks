/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "tema1.hpp"
#include <iostream>

using namespace std;

int *
load_1_svc(struct SensorData *argp, struct svc_req *rqstp)
{
	static int  result = 0;

	/*
	 * insert server code here
	 */
	cout<<argp[0].dataId<<endl;

	return &result;
}
