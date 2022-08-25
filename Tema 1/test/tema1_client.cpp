/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "tema1.hpp"
#include <iostream>
#include <string.h>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>

using namespace std;

struct input {
	char *command;
};
typedef struct input input;

u_long *session_key;
string name;
bool logged = false;

void
tema1_prog_1(char *host)
{
	CLIENT *clnt;
	// int  *result_7;
	// struct input_add  update_1_arg;
	// SensorData  *result_8;
	// struct input_id  read_1_arg;
	// output_getstat  *result_9;
	// struct input_id  get_stat_1_arg;
	// output_getstat  *result_10;
	// struct input_key  get_stat_all_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, TEMA1_PROG, TEMA1_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	while(1) {
		string command;
		cin>>command;

		input* client_request = (input*) malloc (sizeof(input));
		client_request->command = (char *) malloc((strlen(command.c_str())+1)*sizeof(char));
        strcpy(client_request->command, command.c_str());

        if (strcmp(client_request->command, "exit") == 0) {
            printf("Now exiting...\n");
            break;
        }

		if (strcmp(client_request->command, "LOGIN") == 0) {
			cin>>name;

			struct input_login login_1_arg;
			login_1_arg.name = (char *) malloc(strlen(name.c_str()+1)*sizeof(char));
			strcpy(login_1_arg.name, name.c_str());
			session_key = login_1(&login_1_arg, clnt);
			if (session_key == (u_long *) NULL) {
				clnt_perror (clnt, "call failed at LOGIN");
			} else if (*session_key == 1) {
				cout<<"Client already logged!\n";
			}
			cout<<*session_key<<endl;
			logged = true;
		}

		if (strcmp(client_request->command, "LOGOUT") == 0) {
			int *res;
			struct input_key  logout_1_arg;
			logout_1_arg.session_key = *session_key;

			res = logout_1(&logout_1_arg, clnt);
			if (res == (int *) NULL) {
				clnt_perror (clnt, "call failed at LOGOUT");
			} else if (*res == 2) {
				cout<<"Client disconected\n";
			} else if (*res == 1) {
				cout<<"Client already logged! You can't logout if you are not logged in!\n";
			} else {
				cout<<"Server had trouble logging you out! Please try again.\n";
			}
		}

		if (strcmp(client_request->command, "LOAD") == 0) {
			int  *res;
			struct input_load load_1_arg;
			fstream newfile;
			newfile.open(name + ".rpcdb", ios::in);
			if (newfile.is_open()) {
				unsigned line_count = std::count(std::istreambuf_iterator<char>(newfile), std::istreambuf_iterator<char>(), '\n');

				newfile.seekg(0, std::ios::beg);
				
				load_1_arg.nrOfData = line_count+1;
				load_1_arg.session_key = *session_key;
				load_1_arg.data = (SensorData *) malloc((line_count+1)*sizeof(SensorData));
				int i = 0;
				string line;

				while(getline(newfile, line)){
					cout<<line<<endl;
					std::stringstream test(line);
					std::string segment;

					std::getline(test, segment, ' ');
					load_1_arg.data[i].dataId = stoi(segment);
					std::getline(test, segment, ' ');
					load_1_arg.data[i].noValues = stoi(segment);
					load_1_arg.data[i].values = (float *) malloc(load_1_arg.data[i].noValues * sizeof(float));

					for (int j = 0; j < load_1_arg.data[i].noValues; j++) {
						std::getline(test, segment, ' ');
						load_1_arg.data[i].values[j] = std::stof(segment);
						cout<<i<<" "<<j<<" "<<load_1_arg.data[i].values[j]<<endl;
					}

					i++;
				}
				newfile.close();
			}

			res = load_1(&load_1_arg, clnt);
			if (res == (int *) NULL) {
				clnt_perror (clnt, "call failed in LOAD");
			} else if (*res == 1) {
				cout<<"You can't LOAD if you are not logged in!\n";
			}
		}

		if (strcmp(client_request->command, "STORE") == 0) {
			output_store  *res;
			struct input_key store_1_arg;
			store_1_arg.session_key = *session_key;

			res = store_1(&store_1_arg, clnt);
			if (res == (output_store *) NULL) {
				clnt_perror (clnt, "call failed");
			} else if (res->nrOfData == -2) {
				cout<<"You can't STORE if you are not logged in!\n";
			} else if (res->data == NULL) {
				fstream file;
				file.open (name + ".rpcdb", ios::out | ios::trunc );
				if (file.is_open()) {
					file.close();
				}
			} else {
				fstream file;
				file.open (name + ".rpcdb", ios::out | ios::trunc );
				if (file.is_open()) {
					for (int i = 0; i < res->nrOfData; ++i) {
						string line = "";
						line += to_string(res->data[i].dataId);
						line += " " + to_string(res->data[i].noValues);
						for (int j = 0; j < res->data[i].noValues; ++j) {
							line += " " + to_string(res->data[i].values[j]);
						}
						line += "\n";
						file << line;
					}
					file.close();
				}
			}
		}

		if (strcmp(client_request->command, "ADD") == 0) {
			struct input_add add_1_arg;
			add_1_arg.session_key = *session_key;
			int *res;
			cin>>add_1_arg.data.dataId>>add_1_arg.data.noValues;
			add_1_arg.data.values = (float *) malloc(add_1_arg.data.noValues*sizeof(float));
			for (int i = 0; i < add_1_arg.data.noValues; i++) {
				cin>>add_1_arg.data.values[i];
			}
			res = add_1(&add_1_arg, clnt);
			if (res == (int *) NULL) {
				clnt_perror (clnt, "call failed on ADD");
			} else if (*res == 1) {
				cout<<"You can't ADD if you are not logged in!\n";
			} else if (*res == 3) {
				cout<<"Error: The valueId is already in db!\n";
			} else if (*res == 2) {
				cout<<"Values added!\n";
			}
		}

		if (strcmp(client_request->command, "DEL") == 0) {
			struct input_id  del_1_arg;
			del_1_arg.session_key = *session_key;
			int *res;
			cin>>del_1_arg.id;
			res = del_1(&del_1_arg, clnt);
			if (res == (int *) NULL) {
				clnt_perror (clnt, "call failed on DEL");
			} else if (*res == 1) {
				cout<<"You can't DEL if you are not logged in!\n";
			} else if (*res == 3) {
				cout<<"Error: You don't have anything in db!\n";
			} else if (*res == 2) {
				cout<<"Error: The Id is not in db!\n";
			} else {
				cout<<"Item deleted!\n";
			}
		}

		if (strcmp(client_request->command, "UPDATE") == 0) {
			struct input_add  update_1_arg;
			update_1_arg.session_key = *session_key;
			int *res;

			cin>>update_1_arg.data.dataId;
			cin>>update_1_arg.data.noValues;
			update_1_arg.data.values = (float *) malloc(update_1_arg.data.noValues*sizeof(float));
			for (int i = 0; i < update_1_arg.data.noValues; ++i) {
				cin>>update_1_arg.data.values[i];
			}

			res = update_1(&update_1_arg, clnt);
			if (res == (int *) NULL) {
				clnt_perror (clnt, "call failed on DEL");
			} else if (*res == 1) {
				cout<<"You can't UPDATE if you are not logged in!\n";
			} else if (*res == 3) {
				cout<<"You added a new item to db!\n";
			} else if (*res == 2) {
				cout<<"You added a new item to a new db!\n";
			} else {
				cout<<"Item updated!\n";
			}
		}
	}

	// result_7 = update_1(&update_1_arg, clnt);
	// if (result_7 == (int *) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }
	// result_8 = read_1(&read_1_arg, clnt);
	// if (result_8 == (SensorData *) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }
	// result_9 = get_stat_1(&get_stat_1_arg, clnt);
	// if (result_9 == (output_getstat *) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }
	// result_10 = get_stat_all_1(&get_stat_all_1_arg, clnt);
	// if (result_10 == (output_getstat *) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	tema1_prog_1 (host);
exit (0);
}
