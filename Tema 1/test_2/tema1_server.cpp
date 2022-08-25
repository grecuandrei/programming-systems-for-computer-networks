/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "tema1.hpp"
#include <iostream>
#include <stdlib.h>
#include <iterator>
#include <map>

using namespace std;

typedef uint32_t u32;
#define RAND() (rand() & 0x7fff)

map<string, u_long> users;
map<u_long, pair<struct SensorData *, int>> users_db;

int check_login(u_long session_key) {
	bool a = true;
 
    for (auto& it : users) {
        if (it.second == session_key) {
            a = false;
        }
    }
    if (a) {
        return 1;
    }

	return 0;
}

u_long *
login_1_svc(struct input_login *argp, struct svc_req *rqstp)
{
	static unsigned long result = 0;

	if (users.count(argp->name) == 0) {
		srand(time(0));
		result = ((u32)RAND()<<22) ^ ((u32)RAND()<< 9) ^ ((u32)RAND()>> 4);

		users.insert(pair<string, u_long>(argp->name, result));
	} else {
		result = 1;
	}

	cout<<"users login\n";
	map<string, u_long>::iterator itr;
	for (itr = users.begin(); itr != users.end(); ++itr) {
        cout << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }

	return &result;
}

int *
logout_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static int  result = 2;

	u_long session_key = argp->session_key;

	if (check_login(session_key) == 1) {
		result = 1;
		return &result;
	}

	bool a = true;
	string key;
 
    for (auto& it : users) {
        if (it.second == session_key) {
            a = false;
			key = it.first;
        }
    }
    if (!a) {
        users.erase(key);
		if (users_db.count(session_key) != 0) {
			users_db.erase(session_key);
		}
    }

	cout<<"users logout\n";
	map<string, u_long>::iterator itr;
	for (itr = users.begin(); itr != users.end(); ++itr) {
        cout << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }
	cout<<"users_db logout\n";
	map<u_long, pair<struct SensorData *, int>>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first
             << '\t' << itr2->second.first[0].dataId << '\n';
    }

	return &result;
}

int *
load_1_svc(struct input_load *argp, struct svc_req *rqstp)
{
	static int  result;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	cout<<argp->data[0].values[1]<<endl;

	struct SensorData *dataToLoad = (SensorData *) malloc(argp->nrOfData*sizeof(SensorData));
	for (int i = 0; i < argp->nrOfData; ++i) {
		dataToLoad[i].dataId = argp->data[i].dataId;
		dataToLoad[i].noValues = argp->data[i].noValues;
		// cout<<argp->data[i].dataId<<" "<<argp->data[i].noValues<<endl;
		dataToLoad[i].values = (float *) malloc(dataToLoad[i].noValues*sizeof(float));
		for (int j = 0; j < dataToLoad[i].noValues; ++j) {
			cout<<i<<" "<<j<<" "<<argp->data[i].values[j]<<endl;
			dataToLoad[i].values[j] = argp->data[i].values[j];
		}
		// cout<<"--"<<endl;
	}
	users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToLoad, argp->nrOfData)));

	cout<<"\nusers_db load\n";
	map<u_long, pair<struct SensorData *, int>>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << '\n';
		for (int i = 0; i < argp->nrOfData; ++i) {
            cout<< '\t' << itr2->second.first[i].dataId << '\n';
		}
    }

	return &result;
}

output_store *
store_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_store  result;

	if (check_login(argp->session_key) == 1) {
		result.nrOfData = -2;
		return &result;
	}

	if (users_db.count(argp->session_key) != 0) {
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		users_db.erase(argp->session_key);
		result.data = (SensorData *) malloc(existingData.second*sizeof(SensorData));
		for(int i=0; i<existingData.second; i++) {
			result.data[i].dataId = existingData.first[i].dataId;
			result.data[i].noValues = existingData.first[i].noValues;
			result.data[i].values = (float *) malloc(result.data[i].noValues*sizeof(float));
			for (int j = 0; j < result.data[i].noValues; ++j) {
				result.data[i].values[j] = existingData.first[i].values[j];
			}
		}
		result.nrOfData = existingData.second;
	} else {
		result.data = NULL;
		result.nrOfData = 0;
	}


	return &result;
}

int *
add_1_svc(struct input_add *argp, struct svc_req *rqstp)
{
	static int  result;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	// daca nu e in bd
	if (users_db.count(argp->session_key) == 0) {
		struct SensorData *dataToAdd = (SensorData *) malloc(sizeof(SensorData));
		dataToAdd[0].dataId = argp->data.dataId;
		dataToAdd[0].noValues = argp->data.noValues;
		dataToAdd[0].values = (float *) malloc(dataToAdd[0].noValues*sizeof(float));
		for (int j = 0; j < dataToAdd[0].noValues; ++j) {
			dataToAdd[0].values[j] = argp->data.values[j];
		}
		users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, 1)));
	} else { // daca exista bd
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		users_db.erase(argp->session_key);
		struct SensorData *dataToAdd = (SensorData *) malloc((existingData.second+1)*sizeof(SensorData));

		for(int i=0; i<existingData.second; i++) {
			dataToAdd[i].dataId = existingData.first[i].dataId;
			dataToAdd[i].noValues = existingData.first[i].noValues;
			dataToAdd[i].values = (float *) malloc(dataToAdd[i].noValues*sizeof(float));
			for (int j = 0; j < dataToAdd[i].noValues; ++j) {
				dataToAdd[i].values[j] = existingData.first[i].values[j];
			}
		}
		dataToAdd[existingData.second].dataId = argp->data.dataId;
		dataToAdd[existingData.second].noValues = argp->data.noValues;
		dataToAdd[existingData.second].values = (float *) malloc(dataToAdd[existingData.second].noValues*sizeof(float));
		for (int j = 0; j < dataToAdd[existingData.second].noValues; ++j) {
			dataToAdd[existingData.second].values[j] = argp->data.values[j];
		}
		users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, existingData.second+1)));
	}

	return &result;
}

int *
del_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static int  result = 4;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	if (users_db.count(argp->session_key) == 0) {
		result = 3;
	} else {
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		for(int i=0; i < existingData.second; ++i) {
			if (existingData.first[i].dataId == argp->id) {
				struct SensorData *dataToAdd = (SensorData *) malloc((existingData.second-1)*sizeof(SensorData));

				for(int i=0; i<existingData.second && i != argp->id; i++) {
					dataToAdd[i].dataId = existingData.first[i].dataId;
					dataToAdd[i].noValues = existingData.first[i].noValues;
					dataToAdd[i].values = (float *) malloc(dataToAdd[i].noValues*sizeof(float));
					for (int j = 0; j < dataToAdd[i].noValues; ++j) {
						dataToAdd[i].values[j] = existingData.first[i].values[j];
					}
				}
				
				users_db.erase(argp->session_key);
				users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, existingData.second-1)));

				result = 0;
				break;
			}
		}
	}

	if (result == 4) result = 2;

	return &result;
}

int *
update_1_svc(struct SensorData *argp, struct svc_req *rqstp)
{
	static int  result;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	// daca nu are bd
	if (users_db.count(argp->session_key) == 0) {
		struct SensorData *dataToAdd = (SensorData *) malloc(sizeof(SensorData));
		dataToAdd[0].dataId = argp->data.dataId;
		dataToAdd[0].noValues = argp->data.noValues;
		dataToAdd[0].values = (float *) malloc(dataToAdd[0].noValues*sizeof(float));
		for (int j = 0; j < dataToAdd[0].noValues; ++j) {
			dataToAdd[0].values[j] = argp->data.values[j];
		}
		users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, 1)));
		result = 2;
	} else { // are bd
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		for(int i=0; i < existingData.second; ++i) {
			if (existingData.first[i].dataId == argp->id) {
				
			}
		}
	}

	return &result;
}

SensorData *
read_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static SensorData  result;

	/*
	 * insert server code here
	 */

	return &result;
}

output_getstat *
get_stat_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static output_getstat  result;

	/*
	 * insert server code here
	 */

	return &result;
}

output_getstat *
get_stat_all_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_getstat  result;

	/*
	 * insert server code here
	 */

	return &result;
}
