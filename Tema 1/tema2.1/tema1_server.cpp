#include "tema1.hpp"
#include <stdlib.h>
#include <map>
#include <limits>
#include <bits/stdc++.h>

using namespace std;

// map for user-session_key association
map<string, u_long> users_key;
// map for session_key_of_user-his_db association
map<u_long, SensorsData> users_db;

// freeing memory from a user db
void empty_db(u_long session_key) {
	SensorsData existingData = users_db.at(session_key);
	existingData.size = 0;
	for(int i = 0; i < existingData.data_len; ++i) {
		existingData.data_val[i].dataId = 0;
		existingData.data_val[i].noValues = 0;
		free(existingData.data_val[i].values.values_val);
		existingData.data_val[i].values.values_len = 0;
	}
	existingData.data_len = 0;
	users_db.erase(session_key);
}

// checks the user_key db if the user is logged
int check_login(u_long session_key) {
	if (session_key == 0) return 2;  // if the key is 0 the user is not logged

	// checking if the user is logged by searching after session_key
	bool a = true;

    for (auto& it : users_key) {
        if (it.second == session_key) {
            a = false;
        }
    }
    if (a != false) {
        return 1;
    }

	return 0;
}

int resize(u_long session_key) {
	int result = 0;
	SensorsData data = users_db.at(session_key);
	SensorsData new_data;
	new_data.size = data.size * 2;
	new_data.data_len = data.data_len;
	new_data.data_val = (SensorData *) malloc(new_data.size*sizeof(SensorData));
	if(new_data.data_val == NULL) {
		printf("Memory allocation failed\n");
		result = 3;
		return result;
	}

	for (int i = 0; i < new_data.data_len; ++i) {
		new_data.data_val[i].dataId = data.data_val[i].dataId;
		new_data.data_val[i].noValues = data.data_val[i].noValues;
		new_data.data_val[i].values.values_len = new_data.data_val[i].noValues;
		new_data.data_val[i].values.values_val = (float *) malloc(new_data.data_val[i].noValues*sizeof(float));
		if(new_data.data_val[i].values.values_val == NULL) {
			printf("Memory allocation failed\n");
			result = 3;
			return result;
		}
		for (int j = 0; j < new_data.data_val[i].noValues; ++j) {
			new_data.data_val[i].values.values_val[j] = data.data_val[i].values.values_val[j];
		}
	}

	empty_db(session_key);
	users_db.insert(pair<u_long, SensorsData>(session_key, new_data));
	return result;
}

u_long *
login_1_svc(struct input_login *argp, struct svc_req *rqstp)
{
	static unsigned long result = 0;

	if (users_key.count(argp->name) == 0) { // if the user is not already logged
		//generate session_key
		srand(time(0));
		result = ((u32)RAND()<<22) ^ ((u32)RAND()<< 9) ^ ((u32)RAND()>> 4);

		users_key.insert(pair<string, u_long>(argp->name, result));
	} else { // user already logged
		result = 0;
	}

	cout<<"users login\n";
	map<string, u_long>::iterator itr;
	for (itr = users_key.begin(); itr != users_key.end(); ++itr) {
        cout << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }

	return &result;
}

int *
logout_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static int  result = 0;
	u_long session_key = argp->session_key;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1) {
		result = 1;
		return &result;
	} else if (logged == 2) {
		result = 5;
		return &result;
	}

	// get name of user
	bool a = true;
	string key;

    for (auto& it : users_key) {
        if (it.second == session_key) {
            a = false;
			key = it.first;
        }
    }

	// delete entry of user and his db
    if (!a) {
        users_key.erase(key);
		if (users_db.count(session_key) != 0) { // if it has db
			empty_db(session_key);
		}
		result = 2;
    }

	cout<<"users logout\n";
	map<string, u_long>::iterator itr;
	for (itr = users_key.begin(); itr != users_key.end(); ++itr) {
        cout << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }
	cout<<"users_db logout\n";
	map<u_long, SensorsData>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << endl;
		for(int i = 0; i < itr2->second.data_len; ++i) {
            cout << '\t' << itr2->second.data_val[0].dataId << '\n';
		}
    }

	return &result;
}

int *
load_1_svc(struct input_load *argp, struct svc_req *rqstp)
{
	static int  result = 0;
	u_long session_key = argp->session_key;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result = 1;
		return &result;
	}

	if (users_db.count(session_key) == 0) { // db doesn't exists
		// load all data from client
		SensorsData dataToLoad;
		dataToLoad.size = 50;
		dataToLoad.data_len = argp->data.data_len;
		dataToLoad.data_val = (SensorData *) malloc(dataToLoad.size*sizeof(SensorData));
		if(dataToLoad.data_val == NULL) {
			printf("Memory allocation failed\n");
			result = 3;
			return &result;
		}

		for (int i = 0; i < dataToLoad.data_len; ++i) {
			dataToLoad.data_val[i].dataId = argp->data.data_val[i].dataId;
			dataToLoad.data_val[i].noValues = argp->data.data_val[i].noValues;
			dataToLoad.data_val[i].values.values_len = dataToLoad.data_val[i].noValues;
			dataToLoad.data_val[i].values.values_val = (float *) malloc(dataToLoad.data_val[i].noValues*sizeof(float));
			if(dataToLoad.data_val[i].values.values_val == NULL) {
				printf("Memory allocation failed\n");
				result = 3;
				return &result;
			}
			for (int j = 0; j < dataToLoad.data_val[i].noValues; ++j) {
				dataToLoad.data_val[i].values.values_val[j] = argp->data.data_val[i].values.values_val[j];
			}
		}

		users_db.insert(pair<u_long, SensorsData>(argp->session_key, dataToLoad));
	} else { // db exists and u can't load anymore
		result = 2;
	}

	cout<<"\nusers_db load\n";
	map<u_long, SensorsData>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << endl;
		for(int i = 0; i < itr2->second.data_len; ++i) {
            cout << '\t' << itr2->second.data_val[i].dataId << '\n';
		}
    }

	return &result;
}

output_store *
store_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_store result;
	u_long session_key = argp->session_key;
	result.error = 0;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result.error = 1;
		return &result;
	}

	// if db exists
	if (users_db.count(session_key) == 1) {
		// get the data from user db
		SensorsData existingData = users_db.at(session_key);
		//store them in return
		result.data.data_len = existingData.data_len;
		result.data.data_val = (SensorData *) malloc(result.data.data_len*sizeof(SensorData));
		if(result.data.data_val == NULL) {
			printf("Memory allocation failed\n");
			result.error = 3;
			return &result;
		}
		for(int i = 0; i < result.data.data_len; ++i) {
			result.data.data_val[i].dataId = existingData.data_val[i].dataId;
			result.data.data_val[i].noValues = existingData.data_val[i].noValues;
			result.data.data_val[i].values.values_len = result.data.data_val[i].noValues;
			result.data.data_val[i].values.values_val = (float *) malloc(result.data.data_val[i].noValues*sizeof(float));
			if(result.data.data_val == NULL) {
				printf("Memory allocation failed\n");
				result.error = 3;
				return &result;
			}
			for (int j = 0; j < result.data.data_val[i].noValues; ++j) {
				result.data.data_val[i].values.values_val[j] = existingData.data_val[i].values.values_val[j];
			}
		}

		// empty the db of the user since it doesn't need it anymore
		empty_db(session_key);
	} else { // if db doesn't exists
		result.data.data_val = NULL;
		result.error = 2;
	}

	return &result;
}

int *
add_1_svc(struct input_add *argp, struct svc_req *rqstp)
{
	static int  result = 0;
	u_long session_key = argp->session_key;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result = 1;
		return &result;
	}

	// if db doesn't exists
	if (users_db.count(argp->session_key) == 0) {
		// get data from arg
		SensorsData dataToAdd;
		dataToAdd.size = 50;
		dataToAdd.data_len = 1;
		dataToAdd.data_val = (SensorData *) malloc(dataToAdd.size*sizeof(SensorData));
		if(dataToAdd.data_val == NULL) {
			printf("Memory allocation failed\n");
			result = 4;
			return &result;
		}
		dataToAdd.data_val[0].dataId = argp->data.dataId;
		dataToAdd.data_val[0].noValues = argp->data.noValues;
		dataToAdd.data_val[0].values.values_len = dataToAdd.data_val[0].noValues;
		dataToAdd.data_val[0].values.values_val = (float *) malloc(dataToAdd.data_val[0].noValues*sizeof(float));
		if(dataToAdd.data_val[0].values.values_val == NULL) {
			printf("Memory allocation failed\n");
			result = 4;
			return &result;
		}
		for (int j = 0; j < dataToAdd.data_val[0].noValues; ++j) {
			dataToAdd.data_val[0].values.values_val[j] = argp->data.values.values_val[j];
		}

		// make a new db for this item
		users_db.insert(pair<u_long, SensorsData>(argp->session_key, dataToAdd));

		result = 2;
	} else {// if db exists
		// get the data from his db
		SensorsData existingData = users_db.at(argp->session_key);

		for(int i=0; i < existingData.data_len; ++i) {
			// if the item exists in db can't add it again
			if (existingData.data_val[i].dataId == argp->data.dataId) {
				result = 3;
				return &result;
			}
		}
		// if the item doesn't exists in db load data
		SensorsData dataToAdd;
		if (existingData.size < existingData.data_len + 1) {
			resize(session_key);
			existingData = users_db.at(argp->session_key);
		}
		cout<<existingData.data_val[0].dataId<<"D"<<existingData.data_val[1].dataId<<endl;
		existingData.data_val[existingData.data_len].dataId = argp->data.dataId;
		existingData.data_val[existingData.data_len].noValues = argp->data.noValues;
		existingData.data_val[existingData.data_len].values.values_len = argp->data.values.values_len;
		existingData.data_val[existingData.data_len].values.values_val = (float *) malloc(existingData.data_val[existingData.data_len].values.values_len*sizeof(float));
		if(existingData.data_val[existingData.data_len].values.values_val == NULL) {
			printf("Memory allocation failed\n");
			result = 4;
			return &result;
		}
		cout<<existingData.data_val[existingData.data_len].dataId<<" "<<existingData.data_val[existingData.data_len].noValues<<endl;
		for (int j = 0; j < existingData.data_val[existingData.data_len].noValues; ++j) {
			existingData.data_val[existingData.data_len].values.values_val[j] = argp->data.values.values_val[j];
			cout<<existingData.data_val[existingData.data_len].values.values_val[j]<<" ";
		}
		(&existingData)->data_len = (&existingData)->data_len + 1;
		cout<<endl;

		// // clear db from map
		// empty_db(argp->session_key);
		// // insert the new updated db
		// users_db.insert_or_assign(pair<u_long, SensorsData>(argp->session_key, existingData));
		users_db.insert_or_assign(argp->session_key, existingData);
		// dataToAdd.data_len = existingData.data_len + 1;
		// dataToAdd.data_val = (SensorData *) malloc((existingData.data_len + 1)*sizeof(SensorData));
		// if(dataToAdd.data_val == NULL) {
		// 	printf("Memory allocation failed\n");
		// 	result = 4;
		// 	return &result;
		// }

		// // copy existing db
		// for(int i = 0; i < existingData.data_len; ++i) {
		// 	dataToAdd.data_val[i].dataId = existingData.data_val[i].dataId;
		// 	dataToAdd.data_val[i].noValues = existingData.data_val[i].noValues;
		// 	dataToAdd.data_val[i].values.values_len = dataToAdd.data_val[i].noValues;
		// 	dataToAdd.data_val[i].values.values_val = (float *) malloc(dataToAdd.data_val[i].noValues*sizeof(float));
		// 	if(dataToAdd.data_val[i].values.values_val == NULL) {
		// 		printf("Memory allocation failed\n");
		// 		result = 4;
		// 		return &result;
		// 	}
		// 	for (int j = 0; j < dataToAdd.data_val[i].noValues; ++j) {
		// 		dataToAdd.data_val[i].values.values_val[j] = existingData.data_val[i].values.values_val[j];
		// 	}
		// }
		// // add the new item
		// u_int size = existingData.data_len;
		// dataToAdd.data_val[size].dataId = argp->data.dataId;
		// dataToAdd.data_val[size].noValues = argp->data.noValues;
		// dataToAdd.data_val[size].values.values_len = argp->data.values.values_len;
		// dataToAdd.data_val[size].values.values_val = (float *) malloc(dataToAdd.data_val[size].noValues*sizeof(float));
		// if(dataToAdd.data_val[size].values.values_val == NULL) {
		// 	printf("Memory allocation failed\n");
		// 	result = 4;
		// 	return &result;
		// }
		// for (int j = 0; j < dataToAdd.data_val[size].noValues; ++j) {
		// 	dataToAdd.data_val[size].values.values_val[j] = argp->data.values.values_val[j];
		// }

		// // clear db from map
		// empty_db(argp->session_key);
		// // insert the new updated db
		// users_db.insert(pair<u_long, SensorsData>(argp->session_key, dataToAdd));

		result = 0;
	}

	cout<<"\nusers_db add\n";
	map<u_long, SensorsData>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << endl;
		for(int i = 0; i < itr2->second.data_len; ++i) {
            cout << '\t' << itr2->second.data_val[i].dataId << ' ';
		}
    }

	return &result;
}

int *
del_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static int  result = 0;
	u_long session_key = argp->session_key;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result = 1;
		return &result;
	}

	// if bd doesn't exists
	if (users_db.count(argp->session_key) == 0) {
		result = 2;
	} else { // if bd exists
		struct SensorsData existingData = users_db.at(argp->session_key);

		for(int i = 0; i < existingData.data_len; ++i) { // check the item is in db
			if (existingData.data_val[i].dataId == argp->id) { // if it is
				// copy all data from db
				SensorsData dataToAdd;
				dataToAdd.data_len = existingData.data_len - 1;
				dataToAdd.data_val = (SensorData *) malloc((existingData.data_len - 1)*sizeof(SensorData));
				if(dataToAdd.data_val == NULL) {
					printf("Memory allocation failed\n");
					result = 4;
					return &result;
				}
				// except the one the client wants to delete
				for(int j = 0; j < existingData.data_len && existingData.data_val[j].dataId != argp->id; ++j) {
					dataToAdd.data_val[j].dataId = existingData.data_val[j].dataId;
					dataToAdd.data_val[j].noValues = existingData.data_val[j].noValues;
					dataToAdd.data_val[j].values.values_len = dataToAdd.data_val[j].noValues;
					dataToAdd.data_val[j].values.values_val = (float *) malloc(dataToAdd.data_val[j].noValues*sizeof(float));
					if(dataToAdd.data_val == NULL) {
						printf("Memory allocation failed\n");
						result = 4;
						return &result;
					}
					for (int k = 0; k < dataToAdd.data_val[j].noValues; ++k) {
						dataToAdd.data_val[j].values.values_val[k] = existingData.data_val[j].values.values_val[k];
					}
				}

				// clear entry
				empty_db(argp->session_key);
				// insert new db with deleted item
				users_db.insert(pair<u_long, struct SensorsData>(argp->session_key, dataToAdd));

				cout<<"\nusers_db del\n";
				map<u_long, SensorsData>::iterator itr2;
				for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
					cout << '\t' << itr2->first << endl;
					for(int i = 0; i < itr2->second.data_len; ++i) {
						cout << '\t' << itr2->second.data_val[i].dataId << '\n';
					}
				}

				result = 0;
				return &result;
			}
		}

		// if the item is not in db
		result = 3;
	}

	cout<<"\nusers_db del\n";
	map<u_long, SensorsData>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << endl;
		for(int i = 0; i < itr2->second.data_len; ++i) {
            cout << '\t' << itr2->second.data_val[i].dataId << '\n';
		}
    }

	return &result;
}

int *
update_1_svc(struct input_add *argp, struct svc_req *rqstp)
{
	static int  result = 0;
	u_long session_key = argp->session_key;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result = 1;
		return &result;
	}

	// update is like add without the case when update the item values or size
	int *res = add_1_svc(argp, rqstp);
	if (*res == 0) { // if item is added
		result = 3;
		return &result;
	} else if (*res == 1) { // if client is not logged
		result = 1;
		return &result;
	} else if (*res == 2) { // if item is added in a new db
		result = 2;
		return &result;
	} else if (*res == 4) { // if there is a malloc problem
		result = 4;
		return &result;
	} else { // if the item is in bd but needs updated
		// get the data from db
		SensorsData existingData = users_db.at(argp->session_key);
		// copy db
		SensorsData dataToAdd;
		dataToAdd.data_len = existingData.data_len;
		dataToAdd.data_val = (SensorData *) malloc(existingData.data_len*sizeof(SensorData));
		if(dataToAdd.data_val == NULL) {
			printf("Memory allocation failed\n");
			result = 4;
			return &result;
		}
		// without the item that needs updated
		for(int i = 0; i < existingData.data_len && existingData.data_val[i].dataId != argp->data.dataId; ++i) {
			dataToAdd.data_val[i].dataId = existingData.data_val[i].dataId;
			dataToAdd.data_val[i].noValues = existingData.data_val[i].noValues;
			dataToAdd.data_val[i].values.values_len = dataToAdd.data_val[i].noValues;
			dataToAdd.data_val[i].values.values_val = (float *) malloc(dataToAdd.data_val[i].noValues*sizeof(float));
			if(dataToAdd.data_val[i].values.values_val == NULL) {
				printf("Memory allocation failed\n");
				result = 4;
				return &result;
			}
			for (int j = 0; j < dataToAdd.data_val[i].noValues; ++j) {
				dataToAdd.data_val[i].values.values_val[j] = existingData.data_val[i].values.values_val[j];
			}
		}

		// add the updated version of the item
		dataToAdd.data_val[existingData.data_len - 1].dataId = argp->data.dataId;
		dataToAdd.data_val[existingData.data_len - 1].noValues = argp->data.noValues;
		dataToAdd.data_val[existingData.data_len - 1].values.values_len = dataToAdd.data_val[existingData.data_len - 1].noValues;
		dataToAdd.data_val[existingData.data_len - 1].values.values_val = (float *) malloc(dataToAdd.data_val[existingData.data_len - 1].noValues*sizeof(float));
		for (int j = 0; j < dataToAdd.data_val[existingData.data_len - 1].noValues; ++j) {
			dataToAdd.data_val[existingData.data_len - 1].values.values_val[j] = argp->data.values.values_val[j];
		}

		// clear entry
		empty_db(argp->session_key);
		// insert new db with updated item
		users_db.insert(pair<u_long, SensorsData>(argp->session_key, dataToAdd));

		result = 0;
	}

	cout<<"\nusers_db update\n";
	map<u_long, SensorsData>::iterator itr2;
	for (itr2 = users_db.begin(); itr2 != users_db.end(); ++itr2) {
        cout << '\t' << itr2->first << endl;
		for(int i = 0; i < itr2->second.data_len; ++i) {
            cout << '\t' << itr2->second.data_val[i].dataId << '\n';
		}
    }

	return &result;
}

SensorData *
read_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static SensorData result;
	u_long session_key = argp->session_key;
	result.dataId = 0;
	result.noValues = 0;
	result.values.values_len = 0;
	result.values.values_val = NULL;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result.dataId = 1;
		return &result;
	}

	// if bd doesn't exists
	if (users_db.count(argp->session_key) == 0) {
		result.dataId = 2;
	} else { // if bd exists
		// get the items from db
		SensorsData existingData = users_db.at(argp->session_key);
		// check if the item is in db
		for(int i=0; i < existingData.data_len; ++i) {
			// if it is
			if (existingData.data_val[i].dataId == argp->id) {
				result.dataId = existingData.data_val[i].dataId;
				result.noValues = existingData.data_val[i].noValues;
				result.values.values_len = result.noValues;
				result.values.values_val = (float *) malloc(result.noValues*sizeof(float));
				if(result.values.values_val == NULL) {
					printf("Memory allocation failed\n");
					result.dataId = 4;
					return &result;
				}
				for (int j = 0; j < result.noValues; ++j) {
					result.values.values_val[j] = existingData.data_val[i].values.values_val[j];
				}

				return &result;
			}
		}
		// if it's not in the db
		result.dataId = 3;
	}

	return &result;
}

output_getstat *
get_stat_1_svc(struct input_id *argp, struct svc_req *rqstp)
{
	static output_getstat result;
	u_long session_key = argp->session_key;
	result.min = 0.0;
	result.max = 0.0;
	result.average = 0.0;
	result.median = 0.0;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result.min = 0.0;
		result.max = FLT_MAX;
		return &result;
	}

	// if bd doesn't exists
	if (users_db.count(argp->session_key) == 0) {
		result.min = FLT_MAX;
		result.max = 0.0;
	} else {// if bd exists
		// get the items from db
		SensorsData existingData = users_db.at(argp->session_key);

		for(int i = 0; i < existingData.data_len; ++i) {
			// check if the item is in db
			if (existingData.data_val[i].dataId == argp->id) {
				// calculate
				result.min = FLT_MAX;
				result.max = -FLT_MAX;
				result.average = 0.0;
				result.median = 0.0;

				sort(existingData.data_val[i].values.values_val, existingData.data_val[i].values.values_val + existingData.data_val[i].noValues);

				// calculate average
				for(int j = 0; j < existingData.data_val[i].noValues; ++j) {
					result.average += existingData.data_val[i].values.values_val[j];
				}
				result.average = result.average / (float) existingData.data_val[i].noValues;

				// calculate median
				if (existingData.data_val[i].noValues % 2 == 0) {
					int mid = existingData.data_val[i].noValues / 2;
					result.median = ((float) existingData.data_val[i].values.values_val[mid-1] + (float) existingData.data_val[i].values.values_val[mid]) / 2.0;
				} else {
					result.median = (float) existingData.data_val[i].values.values_val[(existingData.data_val[i].noValues / 2)];
				}

				// min
				result.min = (float) existingData.data_val[i].values.values_val[0];
				// max
				result.max = (float) existingData.data_val[i].values.values_val[existingData.data_val[i].noValues - 1];

				return &result;
			}
		}

		// if the item is not in db
		result.min = FLT_MAX;
		result.max = FLT_MAX;
	}

	return &result;
}

output_getstat_all *
get_stat_all_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_getstat_all  result;
	u_long session_key = argp->session_key;
	result.res == 0;
	result.stats.stats_len = 0;
	result.stats.stats_val = NULL;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result.res = 1;
		return &result;
	}

	// if bd doesn't exists
	if (users_db.count(argp->session_key) == 0) {
		result.res = 2;
	} else { // if bd exists
		// get the items from db
		struct SensorsData existingData = users_db.at(argp->session_key);
		// copy all the items from bd in return value
		result.stats.stats_len = existingData.data_len;
		result.stats.stats_val = (output_getstat *) malloc(result.stats.stats_len*sizeof(output_getstat));
		if(result.stats.stats_val == NULL) {
			printf("Memory allocation failed\n");
			result.res = 3;
			return &result;
		}

		// calculate for all the items
		for(int i = 0; i < result.stats.stats_len; ++i) {
			result.stats.stats_val[i].min = FLT_MAX;
			result.stats.stats_val[i].max = -FLT_MAX;
			result.stats.stats_val[i].average = 0.0;
			result.stats.stats_val[i].median = 0.0;

			sort(existingData.data_val[i].values.values_val, existingData.data_val[i].values.values_val + existingData.data_val[i].noValues);

			for(int j=0; j<existingData.data_val[i].noValues; ++j) {
				result.stats.stats_val[i].average += existingData.data_val[i].values.values_val[j];
			}

			if (existingData.data_val[i].noValues % 2 == 0) {
				int mid = existingData.data_val[i].noValues / 2;
				result.stats.stats_val[i].median = ((float) existingData.data_val[i].values.values_val[mid-1] + (float) existingData.data_val[i].values.values_val[mid]) / 2.0;
			} else {
				result.stats.stats_val[i].median = (float) existingData.data_val[i].values.values_val[(existingData.data_val[i].noValues / 2)];
			}

			result.stats.stats_val[i].min = (float) existingData.data_val[i].values.values_val[0];
			result.stats.stats_val[i].max = (float) existingData.data_val[i].values.values_val[existingData.data_val[i].noValues - 1];
			result.stats.stats_val[i].average = result.stats.stats_val[i].average / (float) existingData.data_val[i].noValues;
		}

		result.res = 0;
	}

	return &result;
}

output_store *
read_all_1_svc(struct input_key *argp, struct svc_req *rqstp)
{
	static output_store  result;
	u_long session_key = argp->session_key;
	result.error = 0;
	result.data.data_len = 0;
	result.data.data_val = NULL;

	// checking if user is logged
	int logged = check_login(session_key);
	if (logged == 1 || logged == 2) {
		result.error = 1;
		return &result;
	}

	// if bd doesn't exists
	if (users_db.count(argp->session_key) == 0) {
		result.error = 2;
	} else { // if bd exists
		// get the items from db
		SensorsData existingData = users_db.at(argp->session_key);

		// copy all the items from bd in return value
		cout<<existingData.data_len<<endl;
		result.data.data_len = existingData.data_len;
		result.data.data_val = (SensorData *) malloc((result.data.data_len)*sizeof(SensorData));
		if(result.data.data_val == NULL) {
			printf("Memory allocation failed\n");
			result.error = 3;
			return &result;
		}
		for(int i=0; i < result.data.data_len; ++i) {
			result.data.data_val[i].dataId = existingData.data_val[i].dataId;
			result.data.data_val[i].noValues = existingData.data_val[i].noValues;
			result.data.data_val[i].values.values_len = existingData.data_val[i].noValues;
			result.data.data_val[i].values.values_val = (float *) malloc(existingData.data_val[i].noValues*sizeof(float));
			if(result.data.data_val[i].values.values_val == NULL) {
				printf("Memory allocation failed\n");
				result.error = 3;
				return &result;
			}
			for (int j = 0; j < existingData.data_val[i].noValues; ++j) {
				result.data.data_val[i].values.values_val[j] = existingData.data_val[i].values.values_val[j];
			}
		}
	}

	return &result;
}
