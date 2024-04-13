int open_modern(const gchar *const addr, const gchar *const port,
                GError **const gerror) {
	struct addrinfo hints;
	struct addrinfo* ai = NULL;
	struct addrinfo* ai_bak;
	struct sock_flags;
	int e;
        int retval = -1;
	int i=0;
	int sock = -1;

	memset(&hints, '\0', sizeof(hints));
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	e = getaddrinfo(addr, port ? port : NBD_DEFAULT_PORT, &hints, &ai);
	ai_bak = ai;
	if(e != 0) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_GAI,
                            "failed to open a modern socket: "
                            "failed to get address info: %s",
                            gai_strerror(e));
                goto out;
	}

	while(ai != NULL) {
		sock = -1;

		if((sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol))<0) {
			g_set_error(gerror, NBDS_ERR, NBDS_ERR_SOCKET,
				    "failed to open a modern socket: "
				    "failed to create a socket: %s",
				    strerror(errno));
			goto out;
		}

		if (dosockopts(sock, gerror) == -1) {
			g_prefix_error(gerror, "failed to open a modern socket: ");
			goto out;
		}

		if(bind(sock, ai->ai_addr, ai->ai_addrlen)) {
			/* This is so wrong. 
			 * 
			 * Linux will return multiple entries for the
			 * same system when we ask it for something
			 * AF_UNSPEC, even though the first entry will
			 * listen to both protocols. Other systems will
			 * return multiple entries too, but we actually
			 * do need to open both. Sigh.
			 *
			 * Handle it by ignoring EADDRINUSE if we've
			 * already got at least one socket open
			 */
			if(errno == EADDRINUSE && modernsocks->len > 0) {
				goto next;
			}
			g_set_error(gerror, NBDS_ERR, NBDS_ERR_BIND,
				    "failed to open a modern socket: "
				    "failed to bind an address to a socket: %s",
				    strerror(errno));
			goto out;
		}

		if(listen(sock, 10) <0) {
			g_set_error(gerror, NBDS_ERR, NBDS_ERR_BIND,
				    "failed to open a modern socket: "
				    "failed to start listening on a socket: %s",
				    strerror(errno));
			goto out;
		}
		g_array_append_val(modernsocks, sock);
	next:
		ai = ai->ai_next;
	}

        retval = 0;
out:

        if (retval == -1 && sock >= 0) {
                close(sock);
        }
	if(ai_bak)
		freeaddrinfo(ai_bak);

        return retval;
}
