int dosockopts(const int socket, GError **const gerror) {
#ifndef sun
	int yes=1;
#else
	char yes='1';
#endif /* sun */
	struct linger l;

	/* lose the pesky "Address already in use" error message */
	if (setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_SO_REUSEADDR,
                            "failed to set socket option SO_REUSEADDR: %s",
                            strerror(errno));
                return -1;
	}
	l.l_onoff = 1;
	l.l_linger = 10;
	if (setsockopt(socket,SOL_SOCKET,SO_LINGER,&l,sizeof(l)) == -1) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_SO_LINGER,
                            "failed to set socket option SO_LINGER: %s",
                            strerror(errno));
                return -1;
	}
	if (setsockopt(socket,SOL_SOCKET,SO_KEEPALIVE,&yes,sizeof(int)) == -1) {
                g_set_error(gerror, NBDS_ERR, NBDS_ERR_SO_KEEPALIVE,
                            "failed to set socket option SO_KEEPALIVE: %s",
                            strerror(errno));
                return -1;
	}

        return 0;
}
