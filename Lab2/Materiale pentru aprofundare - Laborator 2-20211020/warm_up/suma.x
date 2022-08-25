struct nod
{
	int x;
	int y;
};

program SUMA_PROG {
	version SUMA_VERS {
		int GET_SUMA(struct nod) = 1;
	} = 1;
} = 1234567;
