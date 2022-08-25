#include "tema1.hpp"
#include <string.h>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

void
tema1_prog_1(char *host, char *input_file)
{
	CLIENT *clnt;

#ifndef	DEBUG
	clnt = clnt_create (host, TEMA1_PROG, TEMA1_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	u_long *session_key = 0;
	string name;
	fstream newfile;
	newfile.open(input_file, ios::in);
	if (newfile.is_open()) {
		string line;

		while(getline(newfile, line)){
			string command;
			istringstream ss(line);
			ss >> command;

			if (command == "exit" || command == "EXIT") {
				printf("Now exiting...\n");
				break;
			}

			if (command == "LOGIN" || command == "login") {
				struct input_login login_1_arg;

				// prepare args
				ss >> name;
				login_1_arg.name = (char *) malloc((strlen(name.c_str())+1)*sizeof(char));
				if(login_1_arg.name == NULL) {
					printf("Memory allocation failed\n");
					exit(0);
				}
				strcpy(login_1_arg.name, name.c_str());

				session_key = login_1(&login_1_arg, clnt);
				// interpretation
				if (session_key == (u_long *) NULL) {
					clnt_perror (clnt, "call failed at LOGIN");
				} else if (*session_key == 0) {
					cout<<"Client already logged!\n";
					session_key = NULL;
					continue;
				}
				cout<<"Client logged! Session key: "<<*session_key<<endl;
				free(login_1_arg.name);
			}

			if (command == "LOGOUT" || command == "logout") {
				int *res;
				struct input_key  logout_1_arg;

				// prepare args
				if (session_key != NULL) {
					logout_1_arg.session_key = *session_key;
				} else {
					logout_1_arg.session_key = 0;
				}

				res = logout_1(&logout_1_arg, clnt);
				// interpretation
				if (res == (int *) NULL) {
					clnt_perror (clnt, "call failed at LOGOUT");
				} else if (*res == 0) {
					cout<<"Server had trouble logging you out! Please try again.\n";
				} else if (*res == 1) {
					cout<<"Client already logged! You can't logout if you are not logged in!\n";
				} else if (*res == 5) {
					cout<<"Client not logged!\n";
				} else {
					cout<<"Client disconected\n";
					session_key = 0;
				}
			}

			if (command == "LOAD" || command == "load") {
				int  *res;
				struct input_load load_1_arg;

				if (session_key != 0) {

					fstream file;
					// open db file where the client has his values
					// same filename as the name from the login
					file.open(name + ".rpcdb", ios::in);

					if (file.is_open()) {
						// prepare args
						//get number of sensor data
						unsigned line_count = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');

						file.seekg(0, std::ios::beg);

						load_1_arg.session_key = *session_key;
						load_1_arg.data.data_len = line_count + 1;
						load_1_arg.data.data_val = (SensorData *) malloc((line_count+1)*sizeof(SensorData));
						if(load_1_arg.data.data_val == NULL) {
							printf("Memory allocation failed\n");
							exit(0);
						}

						int i = 0;
						string line;

						while(getline(file, line)){
							std::stringstream test(line);
							std::string segment;

							std::getline(test, segment, ' ');
							load_1_arg.data.data_val[i].dataId = stoi(segment);
							std::getline(test, segment, ' ');
							load_1_arg.data.data_val[i].noValues = stoi(segment);
							load_1_arg.data.data_val[i].values.values_len = load_1_arg.data.data_val[i].noValues;
							load_1_arg.data.data_val[i].values.values_val = (float *) malloc(load_1_arg.data.data_val[i].noValues * sizeof(float));

							for (int j = 0; j < load_1_arg.data.data_val[i].noValues; j++) {
								std::getline(test, segment, ' ');
								load_1_arg.data.data_val[i].values.values_val[j] = std::stof(segment);
							}

							i++;
						}
						file.close();


						res = load_1(&load_1_arg, clnt);
						// interpretation
						if (res == (int *) NULL) {
							clnt_perror (clnt, "call failed in LOAD");
						} else if (*res == 1) {
							cout<<"You can't LOAD if you are not logged in!\n";
						} else if (*res == 2) {
							cout<<"You can't LOAD if you already did other operations on db! Store first!\n";
						} else if (*res == 3) {
							cout<<"Error on malloc!\n";
							exit (0);
						} else {
							cout<<"Data loaded!\n";
						}

						// free arg
						for (int i = 0; i < load_1_arg.data.data_len; ++i) {
							load_1_arg.data.data_val[i].dataId = 0;
							load_1_arg.data.data_val[i].noValues = 0;
							free(load_1_arg.data.data_val[i].values.values_val);
							load_1_arg.data.data_val[i].values.values_len = 0;
						}
						free(load_1_arg.data.data_val);
						load_1_arg.data.data_len = 0;
						res = NULL;
						free(res);
					} else {
						cout<<"Error opening the file...\n";
					}
				} else {
					cout<<"You can't LOAD if you are not logged in!\n";
				}
			}

			if (command == "STORE" || command == "store") {
				output_store  *res;
				struct input_key store_1_arg;

				// prepare args
				if (session_key != NULL) {
					store_1_arg.session_key = *session_key;
				} else {
					store_1_arg.session_key = 0;
				}

				res = store_1(&store_1_arg, clnt);

				// interpretation
				if (res == (output_store *) NULL) {
					clnt_perror (clnt, "call failed");
				} else if (res->error == 1) {
					cout<<"You can't STORE if you are not logged in!\n";
				} else if (res->error == 2) {
					fstream file;
					file.open (name + ".rpcdb", ios::out | ios::trunc );
					if (file.is_open()) {
						file.close();
					}
					cout<<"File saved!\n";
				} else if (res->error == 3) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					fstream file;
					file.open (name + ".rpcdb", ios::out | ios::trunc );
					if (file.is_open()) {
						for (int i = 0; i < res->data.data_len; ++i) {
							file << res->data.data_val[i].dataId;
							file<< " "<< res->data.data_val[i].noValues;
							for (int j = 0; j < res->data.data_val[i].noValues; ++j) {
								file<< " " << res->data.data_val[i].values.values_val[j];
							}
							if (i != res->data.data_len - 1) file << "\n";
						}
						file.close();
					}
					cout<<"File saved!\n";
				}

				// free
				for (int i = 0; i < res->data.data_len; ++i) {
					res->data.data_val[i].dataId = 0;
					res->data.data_val[i].noValues = 0;
					free(res->data.data_val[i].values.values_val);
					res->data.data_val[i].values.values_len = 0;
				}
				free(res->data.data_val);
				res->data.data_len = 0;
				res = NULL;
				free(res);
			}

			if (command == "ADD" || command == "add") {
				struct input_add add_1_arg;
				int *res;

				// prepare args
				if (session_key != NULL) {
					add_1_arg.session_key = *session_key;
				} else {
					add_1_arg.session_key = 0;
				}

				string word;

				ss >> word;
				add_1_arg.data.dataId = stoi(word);

				ss >> word;
				add_1_arg.data.noValues = stoi(word);

				add_1_arg.data.values.values_len = add_1_arg.data.noValues;
				add_1_arg.data.values.values_val = (float *) malloc(add_1_arg.data.noValues*sizeof(float));
				if(add_1_arg.data.values.values_val == NULL) {
					printf("Memory allocation failed\n");
					exit(0);
				}
				for (int i = 0; i < add_1_arg.data.noValues; i++) {
					ss >> word;
					add_1_arg.data.values.values_val[i] = stof(word);
				}

				res = add_1(&add_1_arg, clnt);
				// interpretation
				if (res == (int *) NULL) {
					clnt_perror (clnt, "call failed on ADD");
				} else if (*res == 1) {
					cout<<"You can't ADD if you are not logged in!\n";
				} else if (*res == 2) {
					cout<<"The data has been added to an empty db!\n";
				} else if (*res == 3) {
					cout<<"The data is already in db!\n";
				} else if (*res == 4) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					cout<<"Values added!\n";
				}

				// free arg
				free(add_1_arg.data.values.values_val);
				add_1_arg.data.values.values_len = 0;
				res = NULL;
			}

			if (command == "DEL" || command == "del") {
				struct input_id  del_1_arg;
				int *res;

				// prepare args
				if (session_key != NULL) {
					del_1_arg.session_key = *session_key;
				} else {
					del_1_arg.session_key = 0;
				}

				string word;
				ss >> word;
				del_1_arg.id = stoi(word);

				res = del_1(&del_1_arg, clnt);
				// interpretation
				if (res == (int *) NULL) {
					clnt_perror (clnt, "call failed on DEL");
				} else if (*res == 1) {
					cout<<"You can't DEL if you are not logged in!\n";
				} else if (*res == 2) {
					cout<<"You don't have anything in db!\n";
				} else if (*res == 3) {
					cout<<"The Id is not in db!\n";
				} else if (*res == 4) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					cout<<"Item deleted!\n";
				}
				res = NULL;
			}

			if (command == "UPDATE" || command == "update") {
				struct input_add update_1_arg;
				int *res;

				// prepare args
				if (session_key != NULL) {
					update_1_arg.session_key = *session_key;
				} else {
					update_1_arg.session_key = 0;
				}

				string word;
				ss >> word;
				update_1_arg.data.dataId = stoi(word);

				ss >> word;
				update_1_arg.data.noValues = stoi(word);

				update_1_arg.data.values.values_len = update_1_arg.data.noValues;
				update_1_arg.data.values.values_val = (float *) malloc(update_1_arg.data.noValues*sizeof(float));
				for (int i = 0; i < update_1_arg.data.noValues; ++i) {
					ss >> word;
					update_1_arg.data.values.values_val[i] = stof(word);
				}

				res = update_1(&update_1_arg, clnt);

				// interpretation
				if (res == (int *) NULL) {
					clnt_perror (clnt, "call failed on DEL");
				} else if (*res == 1) {
					cout<<"You can't UPDATE if you are not logged in!\n";
				} else if (*res == 2) {
					cout<<"You added a new item to a new db!\n";
				} else if (*res == 3) {
					cout<<"Item added in db with update!\n";
				} else if (*res == 4) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					cout<<"Item updated!\n";
				}

				// free arg
				free(update_1_arg.data.values.values_val);
				update_1_arg.data.values.values_len = 0;
				res = NULL;
			}

			if (command == "READ" || command == "read") {
				SensorData  *res;
				struct input_id  read_1_arg;

				// prepare args
				string word;
				ss >> word;
				read_1_arg.id = stoi(word);

				if (session_key != NULL) {
					read_1_arg.session_key = *session_key;
				} else {
					read_1_arg.session_key = 0;
				}

				res = read_1(&read_1_arg, clnt);
				// interpretation
				if (res == (SensorData *) NULL) {
					clnt_perror (clnt, "call failed");
				} else if (res->dataId == 1 && res->noValues == 0) {
					cout<<"You can't READ if you are not logged in!\n";
				} else if (res->dataId == 2 && res->noValues == 0) {
					cout<<"You don't have any items!\n";
				} else if (res->dataId == 3 && res->noValues == 0) {
					cout<<"You don't have this item!\n";
				} else if (res->dataId == 4 && res->noValues == 0) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					cout<<res->dataId<<" "<<res->noValues;
					for (int i = 0; i < res->noValues; ++i) {
						cout<<" "<<res->values.values_val[i];
					}
					cout<<endl;
				}

				// free
				res->dataId = 0;
				res->noValues = 0;
				free(res->values.values_val);
				res->values.values_len = 0;
				res = NULL;
			}

			if (command == "GET_STAT" || command == "get_stat") {
				output_getstat  *res;
				struct input_id  get_stat_1_arg;

				// prepare args
				string word;
				ss >> word;
				get_stat_1_arg.id = stoi(word);

				if (session_key != NULL) {
					get_stat_1_arg.session_key = *session_key;
				} else {
					get_stat_1_arg.session_key = 0;
				}

				res = get_stat_1(&get_stat_1_arg, clnt);
				// interpretation
				if (res == (output_getstat *) NULL) {
					clnt_perror (clnt, "call failed");
				} else if (res->min == 0 && res->max == FLT_MAX) {
					cout<<"You can't GET_STAT if you are not logged in!\n";
				} else if (res->min == FLT_MAX && res->max == 0) {
					cout<<"You don't have any items!\n";
				}  else if (res->min == FLT_MAX && res->max == FLT_MAX) {
					cout<<"You don't have this item!\n";
				} else {
					cout<<"For item: "<<get_stat_1_arg.id<<" we have: min="<<res->min<<" max="<<res->max<<" average="<<res->average<<" median="<<res->median<<endl;
				}
			}

			if (command == "GET_STAT_ALL" || command == "get_stat_all") {
				output_getstat_all  *res;
				struct input_key  get_stat_all_1_arg;

				// prepare args
				if (session_key != NULL) {
					get_stat_all_1_arg.session_key = *session_key;
				} else {
					get_stat_all_1_arg.session_key = 0;
				}

				res = get_stat_all_1(&get_stat_all_1_arg, clnt);
				// interpretation
				if (res == (output_getstat_all *) NULL) {
					clnt_perror (clnt, "call failed");
				} else if (res->res == 1) {
					cout<<"You can't GET_STAT_ALL if you are not logged in!\n";
				} else if (res->res == 2) {
					cout<<"You don't have any items!\n";
				} else if (res->res == 3) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					cout<<"For your items we have:\n";
					for (int i = 0; i < res->stats.stats_len; ++i) {
						cout<<"min="<<res->stats.stats_val[i].min<<" max="<<res->stats.stats_val[i].max<<" average="<<res->stats.stats_val[i].average<<" median="<<res->stats.stats_val[i].median<<endl;
					}
				}

				// free
				for (int i = 0; i < res->stats.stats_len; ++i) {
					res->stats.stats_val[i].min = 0;
					res->stats.stats_val[i].max = 0;
					res->stats.stats_val[i].average = 0;
					res->stats.stats_val[i].median = 0;
				}
				free(res->stats.stats_val);
				res->stats.stats_len = 0;
				res->res = 0;
				res = NULL;
			}

			if (command == "READ_ALL" || command == "read_all") {
				output_store  *res;
				struct input_key  read_all_1_arg;

				// prepare args
				if (session_key != NULL) {
					read_all_1_arg.session_key = *session_key;
				} else {
					read_all_1_arg.session_key = 0;
				}

				res = read_all_1(&read_all_1_arg, clnt);
				// interpretation
				if (res == (output_store *) NULL) {
					clnt_perror (clnt, "call failed");
				} else if (res->error == 1) {
					cout<<"You can't READ_ALL if you are not logged in!\n";
				} else if (res->error == 2) {
					cout<<"You don't have any items!\n";
				} else if (res->error == 3) {
					cout<<"Error on malloc!\n";
					exit (0);
				} else {
					cout<<"These are your items:\n";
					for (int i = 0; i < res->data.data_len; ++i) {
						cout<<res->data.data_val[i].dataId<<" "<<res->data.data_val[i].noValues;
						for (int j = 0; j < res->data.data_val[i].noValues; ++j) {
							cout<<" "<<res->data.data_val[i].values.values_val[j];
						}
						cout<<endl;
					}
				}

				// free
				for (int i = 0; i < res->data.data_len; ++i) {
					res->data.data_val[i].dataId = 0;
					res->data.data_val[i].noValues = 0;
					free(res->data.data_val[i].values.values_val);
					res->data.data_val[i].values.values_len = 0;
				}
				free(res->data.data_val);
				res->data.data_len = 0;
				res->error = 0;
			}
		}
		newfile.close();
	} else {
		cout<<"Error opening the file...\n";
	}

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host, *input_file;

	if (argc < 3) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	input_file = argv[2]; // file from where the client gets his commands
	tema1_prog_1 (host, input_file);
exit (0);
}
