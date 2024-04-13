int setup_serve(SERVER *const serve, GError **const gerror) {
	struct addrinfo hints;
	struct addrinfo *ai = NULL;
	gchar *port = NULL;
	int e;
        int retval = -1;

        /* Without this, it's possible that socket == 0, even if it's
         * not initialized at all. And that would be wrong because 0 is
         * totally legal value for properly initialized descriptor. This
         * line is required to ensure that unused/uninitialized
         * descriptors are marked as such (new style configuration
         * case). Currently, servers are being initialized in multiple
         * places, and some of the them do the socket initialization
         * incorrectly. This is the only point common to all code paths,
         * and therefore setting -1 is put here. However, the whole
         * server initialization procedure should be extracted to its
         * own function and all code paths wanting to mess with servers
         * should initialize servers with that function.
         * 
         * TODO: fix server initialization */
        serve->socket = -1;

	if(!(glob_flags & F_OLDSTYLE)) {
		return serve->servename ? 1 : 0;
	}
	memset(&hints,'\0',sizeof(hints));
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = serve->socket_family;

	port = g_strdup_printf("%d", serve->port);
	if (!port) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_SYS,
                            "failed to open an export socket: "
                            "failed to convert a port number to a string: %s",
                            strerror(errno));
                goto out;
        }

	e = getaddrinfo(serve->listenaddr,port,&hints,&ai);

	g_free(port);

	if(e != 0) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_GAI,
                            "failed to open an export socket: "
                            "failed to get address info: %s",
                            gai_strerror(e));
                goto out;
	}

	if(serve->socket_family == AF_UNSPEC)
		serve->socket_family = ai->ai_family;

#ifdef WITH_SDP
	if ((serve->flags) && F_SDP) {
		if (ai->ai_family == AF_INET)
			ai->ai_family = AF_INET_SDP;
		else (ai->ai_family == AF_INET6)
			ai->ai_family = AF_INET6_SDP;
	}
#endif
	if ((serve->socket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_SOCKET,
                            "failed to open an export socket: "
                            "failed to create a socket: %s",
                            strerror(errno));
                goto out;
        }

	if (dosockopts(serve->socket, gerror) == -1) {
                g_prefix_error(gerror, "failed to open an export socket: ");
                goto out;
        }

	DEBUG("Waiting for connections... bind, ");
	e = bind(serve->socket, ai->ai_addr, ai->ai_addrlen);
	if (e != 0 && errno != EADDRINUSE) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_BIND,
                            "failed to open an export socket: "
                            "failed to bind an address to a socket: %s",
                            strerror(errno));
                goto out;
        }
	DEBUG("listen, ");
	if (listen(serve->socket, 1) < 0) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_BIND,
                            "failed to open an export socket: "
                            "failed to start listening on a socket: %s",
                            strerror(errno));
                goto out;
        }

        retval = serve->servename ? 1 : 0;
out:

        if (retval == -1 && serve->socket >= 0) {
                close(serve->socket);
                serve->socket = -1;
        }
	freeaddrinfo (ai);

        return retval;
}
