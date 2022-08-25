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
#include <limits>
#include <bits/stdc++.h>
#include <cfloat>

using namespace std;

typedef uint32_t u32;
#define RAND() (rand() & 0x7fff)

map<string, u_long> users;
map<u_long, pair<struct SensorData *, int>> users_db;

int check_login(u_long session_key) {
	if (session_key == 0) return 1;
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
	static int  result = 2;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	if (users_db.count(argp->session_key) == 1) {
		result = 3;
		return &result;
	}

	struct SensorData *dataToLoad = (SensorData *) malloc(argp->nrOfData*sizeof(SensorData));
	for (int i = 0; i < argp->nrOfData; ++i) {
		dataToLoad[i].dataId = argp->data.data_val[i].dataId;
		dataToLoad[i].noValues = argp->data.data_val[i].noValues;
		dataToLoad[i].values.values_len = dataToLoad[i].noValues;
		dataToLoad[i].values.values_val = (float *) malloc(dataToLoad[i].noValues*sizeof(float));
		for (int j = 0; j < dataToLoad[i].noValues; ++j) {
			dataToLoad[i].values.values_val[j] = argp->data.data_val[i].values.values_val[j];
		}
	}
	users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToLoad, argp->nrOfData)));

	cout<<"\nusers_db load\n";
	map<u_long, pair<struct SensorData *, int>>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << '\n';
		for (int i = 0; i < itr2->second.second; ++i) {
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

	// daca exista db
	if (users_db.count(argp->session_key) == 1) {
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		result.data.data_len = existingData.second;
		result.data.data_val = (SensorData *) malloc(existingData.second*sizeof(SensorData));
		for(int i=0; i<existingData.second; i++) {
			result.data.data_val[i].dataId = existingData.first[i].dataId;
			result.data.data_val[i].noValues = existingData.first[i].noValues;
			result.data.data_val[i].values.values_len = result.data.data_val[i].noValues;
			result.data.data_val[i].values.values_val = (float *) malloc(result.data.data_val[i].noValues*sizeof(float));
			for (int j = 0; j < result.data.data_val[i].noValues; ++j) {
				result.data.data_val[i].values.values_val[j] = existingData.first[i].values.values_val[j];
			}
		}
		result.nrOfData = existingData.second;
		users_db.erase(argp->session_key);
	} else { // daca nu exista db
		result.data.data_val = NULL;
		result.nrOfData = 0;
	}

	return &result;
}

int *
add_1_svc(struct input_add *argp, struct svc_req *rqstp)
{
	static int  result = 4;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	// daca nu exista bd
	if (users_db.count(argp->session_key) == 0) {
		struct SensorData *dataToAdd = (SensorData *) malloc(1*sizeof(SensorData));
		dataToAdd[0].dataId = argp->data.dataId;
		dataToAdd[0].noValues = argp->data.noValues;
		dataToAdd[0].values.values_len = dataToAdd[0].noValues;
		dataToAdd[0].values.values_val = (float *) malloc(dataToAdd[0].noValues*sizeof(float));
		for (int j = 0; j < dataToAdd[0].noValues; ++j) {
			dataToAdd[0].values.values_val[j] = argp->data.values.values_val[j];
		}

		users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, 1)));
		result = 2;
	} else { // daca exista bd
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		for(int i=0; i < existingData.second; ++i) {
			// daca exista in bd
			if (existingData.first[i].dataId == argp->data.dataId) {
				result = 3;
				return &result;
			}
		}
		// daca nu exista in bd
		if (result == 4) {
			struct SensorData *dataToAdd = (SensorData *) malloc((existingData.second+1)*sizeof(SensorData));

			for(int i=0; i<existingData.second; i++) {
				dataToAdd[i].dataId = existingData.first[i].dataId;
				dataToAdd[i].noValues = existingData.first[i].noValues;
				dataToAdd[i].values.values_len = dataToAdd[i].noValues;
				dataToAdd[i].values.values_val = (float *) malloc(dataToAdd[i].noValues*sizeof(float));
				for (int j = 0; j < dataToAdd[i].noValues; ++j) {
					dataToAdd[i].values.values_val[j] = existingData.first[i].values.values_val[j];
				}
			}
			dataToAdd[existingData.second].dataId = argp->data.dataId;
			dataToAdd[existingData.second].noValues = argp->data.noValues;
			dataToAdd[existingData.second].values.values_len = argp->data.values.values_len;
			dataToAdd[existingData.second].values.values_val = (float *) malloc(dataToAdd[existingData.second].noValues*sizeof(float));
			for (int j = 0; j < dataToAdd[existingData.second].noValues; ++j) {
				dataToAdd[existingData.second].values.values_val[j] = argp->data.values.values_val[j];
			}

			users_db.erase(argp->session_key);
			users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, existingData.second+1)));
			
			result = 2;
		}
	}

	cout<<"\nusers_db add\n";
	map<u_long, pair<struct SensorData *, int>>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << '\n';
		for (int i = 0; i < 1; ++i) {
            cout<< '\t' << itr2->second.first[i].dataId << " "<<itr2->second.first[i].values.values_val[0]<<'\n';
		}
    }

	return &result;
}

int *
del_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static int  result = 2;

	if (check_login(argp->session_key) == 1) {
		result = 1;
		return &result;
	}

	// daca nu exista bd
	if (users_db.count(argp->session_key) == 0) {
		result = 3;
	} else { // daca exista bd
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		for(int i=0; i < existingData.second; ++i) {
			// daca exista intrarea
			if (existingData.first[i].dataId == argp->id) {
				struct SensorData *dataToAdd = (SensorData *) malloc((existingData.second-1)*sizeof(SensorData));

				for(int i=0; i<existingData.second && i != argp->id; i++) {
					dataToAdd[i].dataId = existingData.first[i].dataId;
					dataToAdd[i].noValues = existingData.first[i].noValues;
					dataToAdd[i].values.values_len = dataToAdd[i].noValues;
					dataToAdd[i].values.values_val = (float *) malloc(dataToAdd[i].noValues*sizeof(float));
					for (int j = 0; j < dataToAdd[i].noValues; ++j) {
						dataToAdd[i].values.values_val[j] = existingData.first[i].values.values_val[j];
					}
				}
				
				users_db.erase(argp->session_key);
				users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, existingData.second-1)));

				result = 0;
				return &result;
			}
		}

		// nu exista id-ul in bd
		result = 4;
	}

	cout<<"\nusers_db del\n";
	map<u_long, pair<struct SensorData *, int>>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << '\n';
		for (int i = 0; i < 1; ++i) {
            cout<< '\t' << itr2->second.first[i].dataId << " "<<itr2->second.first[i].values.values_val[0]<<'\n';
		}
    }

	return &result;
}

int *
update_1_svc(struct input_add *argp, struct svc_req *rqstp)
{
	static int  result;

	// if (check_login(argp->session_key) == 1) {
	// 	result = 1;
	// 	return &result;
	// }
	cout<<"dA\n";

	// // daca nu are bd
	// if (users_db.count(argp->session_key) == 0) {
	// 	struct SensorData *dataToAdd = (SensorData *) malloc(sizeof(SensorData));
	// 	dataToAdd[0].dataId = argp->data.dataId;
	// 	dataToAdd[0].noValues = argp->data.noValues;
	// 	dataToAdd[0].values.values_len = 1;
	// 	dataToAdd[0].values.values_val = (float *) malloc(dataToAdd[0].noValues*sizeof(float));
	// 	for (int j = 0; j < dataToAdd[0].noValues; ++j) {
	// 		dataToAdd[0].values.values_val[j] = argp->data.values.values_val[j];
	// 	}
	// 	users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, 1)));
	// 	result = 2;
	// } else { // are bd
	// 	pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
	// 	cout<<"ARE\n";
	// 	for(int i=0; i < existingData.second; ++i) {
	// 		if (existingData.first[i].dataId == argp->data.dataId) {
	// 			cout<<"EXISTA\n";
	// 			struct SensorData *dataToAdd = (SensorData *) malloc((existingData.second)*sizeof(SensorData));

	// 			for(int i=0; i<existingData.second && i != argp->data.dataId; i++) {
	// 				dataToAdd[i].dataId = existingData.first[i].dataId;
	// 				dataToAdd[i].noValues = existingData.first[i].noValues;
	// 				dataToAdd[i].values.values_len = existingData.first[i].noValues;
	// 				dataToAdd[i].values.values_val = (float *) malloc(dataToAdd[i].noValues*sizeof(float));
	// 				for (int j = 0; j < dataToAdd[i].noValues; ++j) {
	// 					dataToAdd[i].values.values_val[j] = existingData.first[i].values.values_val[j];
	// 				}
	// 			}

	// 			dataToAdd[existingData.second].dataId = argp->data.dataId;
	// 			dataToAdd[existingData.second].noValues = argp->data.noValues;
	// 			dataToAdd[existingData.second].values.values_len = dataToAdd[existingData.second].noValues;
	// 			dataToAdd[existingData.second].values.values_val = (float *) malloc(dataToAdd[existingData.second].noValues*sizeof(float));
	// 			for (int j = 0; j < dataToAdd[existingData.second].noValues; ++j) {
	// 				dataToAdd[existingData.second].values.values_val[j] = argp->data.values.values_val[j];
	// 			}

	// 			users_db.erase(argp->session_key);
	// 			users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, existingData.second)));

	// 			result = 0;
	// 		}
	// 	}

	// 	if (result != 0) { // nu exista in bd dar exista bd
	// 		cout<<"NU EXISTA\n";
	// 		struct SensorData *dataToAdd = (SensorData *) malloc((existingData.second+1)*sizeof(SensorData));

	// 		for(int i=0; i<existingData.second; i++) {
	// 			dataToAdd[i].dataId = existingData.first[i].dataId;
	// 			dataToAdd[i].noValues = existingData.first[i].noValues;
	// 			dataToAdd[i].values.values_len = dataToAdd[i].noValues;
	// 			dataToAdd[i].values.values_val = (float *) malloc(dataToAdd[i].noValues*sizeof(float));
	// 			for (int j = 0; j < dataToAdd[i].noValues; ++j) {
	// 				dataToAdd[i].values.values_val[j] = existingData.first[i].values.values_val[j];
	// 			}
	// 		}

	// 		dataToAdd[existingData.second].dataId = argp->data.dataId;
	// 		dataToAdd[existingData.second].noValues = argp->data.noValues;
	// 		dataToAdd[existingData.second].values.values_len = dataToAdd[existingData.second].noValues;
	// 		dataToAdd[existingData.second].values.values_val = (float *) malloc(dataToAdd[existingData.second].noValues*sizeof(float));
	// 		for (int j = 0; j < dataToAdd[existingData.second].noValues; ++j) {
	// 			dataToAdd[existingData.second].values.values_val[j] = argp->data.values.values_val[j];
	// 		}
	// 		users_db.insert(pair<u_long, pair<struct SensorData *, int>>(argp->session_key, std::make_pair(dataToAdd, existingData.second+1)));

	// 		result = 3;
	// 	}
	// }

	// cout<<"\nusers_db update\n";
	// map<u_long, pair<struct SensorData *, int>>::iterator itr2;
	// for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
    //     cout << '\t' << itr2->first << '\n';
	// 	for (int i = 0; i < 1; ++i) {
    //         cout<< '\t' << itr2->second.first[i].dataId << " "<<itr2->second.first[i].values.values_val[1]<<'\n';
	// 	}
    // }

	return &result;
}

SensorData *
read_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static SensorData result;

	if (check_login(argp->session_key) == 1) {
		result.dataId = 1;
		result.noValues = 0;
		result.values.values_len = 0;
		result.values.values_val = NULL;
		return &result;
	}

	// daca nu exista bd
	if (users_db.count(argp->session_key) == 0) {
		result.dataId = 0;
		result.noValues = 0;
		result.values.values_len = 0;
		result.values.values_val = NULL;
	} else { // daca exista bd
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		for(int i=0; i < existingData.second; ++i) {
			// daca exista intrarea
			if (existingData.first[i].dataId == argp->id) {
				result.dataId = existingData.first[i].dataId;
				result.noValues = existingData.first[i].noValues;
				result.values.values_len = existingData.first[i].noValues;
				result.values.values_val = (float *) malloc(existingData.first[i].noValues*sizeof(float));
				for (int j = 0; j < existingData.first[i].noValues; ++j) {
					result.values.values_val[j] = existingData.first[i].values.values_val[j];
				}
				
				return &result;
			}
		}

		result.dataId = 3;
		result.noValues = 0;
		result.values.values_len = 0;
		result.values.values_val = NULL;
	}

	return &result;
}

output_getstat *
get_stat_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static output_getstat  result;

	if (check_login(argp->session_key) == 1) {
		result.min = 0;
		result.max = FLT_MAX;
		result.average = 0;
		result.median = 0;
		return &result;
	}

	// daca nu exista bd
	if (users_db.count(argp->session_key) == 0) {
		result.min = FLT_MAX;
		result.max = 0;
		result.average = 0;
		result.median = 0;
	} else { // daca exista bd
		pair<struct SensorData *, int> existingData = users_db.at(argp->session_key);
		for(int i=0; i < existingData.second; ++i) {
			// daca exista intrarea
			if (existingData.first[i].dataId == argp->id) {
				result.min = FLT_MAX;
				result.max = -FLT_MAX;
				result.average = 0.0;
				result.median = 0.0;

				sort(existingData.first[i].values.values_val, existingData.first[i].values.values_val + existingData.first[i].noValues);

				for(int j=0; j<existingData.first[i].noValues; j++) {
					result.average += existingData.first[i].values.values_val[j];
				}

				if (existingData.first[i].noValues % 2 == 0) {
					int mid = existingData.first[i].noValues / 2;
					result.median = ((float) existingData.first[i].values.values_val[mid-1] + (float) existingData.first[i].values.values_val[mid]) / 2.0;
				} else {
					result.median = (float) existingData.first[i].values.values_val[(existingData.first[i].noValues / 2)];
				}

				result.min = (float) existingData.first[i].values.values_val[0];
				result.max = (float) existingData.first[i].values.values_val[existingData.first[i].noValues - 1];
				result.average = result.average / (float) existingData.first[i].noValues;
				
				return &result;
			}
		}

		result.min = FLT_MAX;
		result.max = FLT_MAX;
		result.average = 0;
		result.median = 0;
	}

	return &result;
}

output_getstat_all *
get_stat_all_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_getstat_all  result;

	/*
	 * insert server code here
	 */

	return &result;
}

output_store *
read_all_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_store  result;

	/*
	 * insert server code here
	 */

	return &result;
}

