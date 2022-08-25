/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _TEMA1_H_RPCGEN
#define _TEMA1_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct SensorData {
	int dataId;
	int noValues;
	float *values;
};
typedef struct SensorData SensorData;

struct input_login {
	char *name;
};
typedef struct input_login input_login;

struct input_key {
	u_long session_key;
};
typedef struct input_key input_key;

struct input_id {
	int id;
	u_long session_key;
};
typedef struct input_id input_id;

struct input_load {
	int nrOfData;
	u_long session_key;
	struct SensorData *data;
};
typedef struct input_load input_load;

struct input_add {
	u_long session_key;
	struct SensorData data;
};
typedef struct input_add input_add;

struct output_getstat {
	float min;
	float max;
	float average;
	float median;
};
typedef struct output_getstat output_getstat;

struct output_store {
	int nrOfData;
	struct SensorData *data;
};
typedef struct output_store output_store;

#define TEMA1_PROG 0x13112021
#define TEMA1_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define LOGIN 1
extern  u_long * login_1(struct input_login *, CLIENT *);
extern  u_long * login_1_svc(struct input_login *, struct svc_req *);
#define LOGOUT 2
extern  int * logout_1(struct input_key *, CLIENT *);
extern  int * logout_1_svc(struct input_key *, struct svc_req *);
#define LOAD 3
extern  int * load_1(struct input_load *, CLIENT *);
extern  int * load_1_svc(struct input_load *, struct svc_req *);
#define STORE 4
extern  output_store * store_1(struct input_key *, CLIENT *);
extern  output_store * store_1_svc(struct input_key *, struct svc_req *);
#define ADD 5
extern  int * add_1(struct input_add *, CLIENT *);
extern  int * add_1_svc(struct input_add *, struct svc_req *);
#define DEL 6
extern  int * del_1(struct input_id *, CLIENT *);
extern  int * del_1_svc(struct input_id *, struct svc_req *);
#define UPDATE 7
extern  int * update_1(struct SensorData *, CLIENT *);
extern  int * update_1_svc(struct SensorData *, struct svc_req *);
#define READ 8
extern  SensorData * read_1(struct input_id *, CLIENT *);
extern  SensorData * read_1_svc(struct input_id *, struct svc_req *);
#define GET_STAT 9
extern  output_getstat * get_stat_1(struct input_id *, CLIENT *);
extern  output_getstat * get_stat_1_svc(struct input_id *, struct svc_req *);
#define GET_STAT_ALL 10
extern  output_getstat * get_stat_all_1(struct input_key *, CLIENT *);
extern  output_getstat * get_stat_all_1_svc(struct input_key *, struct svc_req *);
extern int tema1_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define LOGIN 1
extern  u_long * login_1();
extern  u_long * login_1_svc();
#define LOGOUT 2
extern  int * logout_1();
extern  int * logout_1_svc();
#define LOAD 3
extern  int * load_1();
extern  int * load_1_svc();
#define STORE 4
extern  output_store * store_1();
extern  output_store * store_1_svc();
#define ADD 5
extern  int * add_1();
extern  int * add_1_svc();
#define DEL 6
extern  int * del_1();
extern  int * del_1_svc();
#define UPDATE 7
extern  int * update_1();
extern  int * update_1_svc();
#define READ 8
extern  SensorData * read_1();
extern  SensorData * read_1_svc();
#define GET_STAT 9
extern  output_getstat * get_stat_1();
extern  output_getstat * get_stat_1_svc();
#define GET_STAT_ALL 10
extern  output_getstat * get_stat_all_1();
extern  output_getstat * get_stat_all_1_svc();
extern int tema1_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_SensorData (XDR *, SensorData*);
extern  bool_t xdr_input_login (XDR *, input_login*);
extern  bool_t xdr_input_key (XDR *, input_key*);
extern  bool_t xdr_input_id (XDR *, input_id*);
extern  bool_t xdr_input_load (XDR *, input_load*);
extern  bool_t xdr_input_add (XDR *, input_add*);
extern  bool_t xdr_output_getstat (XDR *, output_getstat*);
extern  bool_t xdr_output_store (XDR *, output_store*);

#else /* K&R C */
extern bool_t xdr_SensorData ();
extern bool_t xdr_input_login ();
extern bool_t xdr_input_key ();
extern bool_t xdr_input_id ();
extern bool_t xdr_input_load ();
extern bool_t xdr_input_add ();
extern bool_t xdr_output_getstat ();
extern bool_t xdr_output_store ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_TEMA1_H_RPCGEN */
