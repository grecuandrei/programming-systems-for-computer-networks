
Tema 1 - Aplicatie Client/Server pentru baze de date folosind RPC

# Compiling

    The Makefile is already modified for c++ and will NOT run rpcgen.
    You should only run:
        
        make

    To clear the working space (and will not get read of the .cpp files):
        
        make clean

# Running

    ## Client
        
        sudo ./tema1_client 127.0.0.1 input.in

    Where "input" is the name of the file with commands

    ## Server
        
        sudo ./tema1_server

# Commands

    In the input file you can use commands like:

        login
        logout
        load
        store
        add id no_values values
        update id no_values values
        del id
        read id
        read_all
        get_stat id
        get_stat_all
        exit (closes the client - it is used only at the END of the input file)

    and their CAPS verisons(LOAD, LOGIN, ...)

# Filetree

    .cpp/.hpp files - for running the application
    tema1.x - the initial file
    client_name.rpcdb - database files - one per client
    input_file_name.in - files for command input