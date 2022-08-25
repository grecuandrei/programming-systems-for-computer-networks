/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "suma.h"


void suma_prog_1(char *host, int x, int y)
{
	CLIENT *clnt;
	int  *result_1;
	struct nod  get_suma_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, SUMA_PROG, SUMA_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	get_suma_1_arg.x = x;
	get_suma_1_arg.y = y;
	result_1 = get_suma_1(&get_suma_1_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	printf("Serverul a calculat... suma este: %d\n", *result_1);
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int main (int argc, char *argv[])
{
	char *host;
	int x;
	int y;

	if (argc < 2) {
		printf ("usage: %s server_host nr1 nr2\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	x = atoi(argv[2]);
	y = atoi(argv[3]);

	suma_prog_1 (host, x, y);
exit (0);
}