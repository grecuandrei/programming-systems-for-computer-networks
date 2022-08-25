struct SensorData
{
	int dataId;
    int noValues;
    float *values;
};

struct input_login {
    string name;
};

struct input_id {
    int id;
};

struct output_getstat {
    float min;
    float max;
    float average;
    float median;
};

program TEMA1_PROG {
	version TEMA1_VERS {
		unsigned long LOGIN(input_login) = 1;
        int LOGOUT() = 2;
        int LOAD() = 3;
        int STORE() = 4
        int ADD(SensorData) = 5;
        int DEL(input_id) = 6;
        int UPDATE(SensorData) = 7;
        SensorData READ(input_id) = 8;
        output_getstat GET_STAT(input_id) = 9;
        output_getstat GET_STAT_ALL() = 10;
	} = 1;
} = 0x13112021;