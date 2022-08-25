struct SensorData
{
	int dataId;
    int noValues;
    float values<>;
};

struct input_login
{
    string name<>;
};

struct input_key
{
    unsigned long session_key;
};

struct input_id
{
    int id;
    unsigned long session_key;
};

struct input_load
{
    int nrOfData;
    unsigned long session_key;
    struct SensorData data<>;
};

struct input_add
{
    unsigned long session_key;
    struct SensorData data;
};

struct output_getstat
{
    float min;
    float max;
    float average;
    float median;
};

struct output_store
{
    int nrOfData;
    struct SensorData data<>;
};

program TEMA1_PROG {
	version TEMA1_VERS {
		unsigned long LOGIN(struct input_login) = 1;
        int LOGOUT(struct input_key) = 2;
        int LOAD(struct input_load) = 3;
        output_store STORE(struct input_key) = 4;
        int ADD(struct input_add) = 5;
        int DEL(struct input_id) = 6;
        int UPDATE(struct input_add) = 7;
        SensorData READ(struct input_id) = 8;
        output_getstat GET_STAT(struct input_id) = 9;
        output_getstat GET_STAT_ALL(struct input_key) = 10;
	} = 1;
} = 0x13112021;