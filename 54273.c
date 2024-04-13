traverse_socks5 (int print_fd, int sok, char *serverAddr, int port)
{
	struct sock5_connect1 sc1;
	unsigned char *sc2;
	unsigned int packetlen, addrlen;
	unsigned char buf[260];
	int auth = prefs.hex_net_proxy_auth && prefs.hex_net_proxy_user[0] && prefs.hex_net_proxy_pass[0];

	sc1.version = 5;
	sc1.nmethods = 1;
	if (auth)
		sc1.method = 2;  /* Username/Password Authentication (UPA) */
	else
		sc1.method = 0;  /* NO Authentication */
	send (sok, (char *) &sc1, 3, 0);
	if (recv (sok, buf, 2, 0) != 2)
		goto read_error;

	if (buf[0] != 5)
	{
		proxy_error (print_fd, "SOCKS\tServer is not socks version 5.\n");
		return 1;
	}

	/* did the server say no auth required? */
	if (buf[1] == 0)
		auth = 0;

	if (auth)
	{
		int len_u=0, len_p=0;

		/* authentication sub-negotiation (RFC1929) */
		if (buf[1] != 2)  /* UPA not supported by server */
		{
			proxy_error (print_fd, "SOCKS\tServer doesn't support UPA authentication.\n");
			return 1;
		}

		memset (buf, 0, sizeof(buf));

		/* form the UPA request */
		len_u = strlen (prefs.hex_net_proxy_user);
		len_p = strlen (prefs.hex_net_proxy_pass);
		buf[0] = 1;
		buf[1] = len_u;
		memcpy (buf + 2, prefs.hex_net_proxy_user, len_u);
		buf[2 + len_u] = len_p;
		memcpy (buf + 3 + len_u, prefs.hex_net_proxy_pass, len_p);

		send (sok, buf, 3 + len_u + len_p, 0);
		if ( recv (sok, buf, 2, 0) != 2 )
			goto read_error;
		if ( buf[1] != 0 )
		{
			proxy_error (print_fd, "SOCKS\tAuthentication failed. "
							 "Is username and password correct?\n");
			return 1; /* UPA failed! */
		}
	}
	else
	{
		if (buf[1] != 0)
		{
			proxy_error (print_fd, "SOCKS\tAuthentication required but disabled in settings.\n");
			return 1;
		}
	}

	addrlen = strlen (serverAddr);
	packetlen = 4 + 1 + addrlen + 2;
	sc2 = malloc (packetlen);
	sc2[0] = 5;						  /* version */
	sc2[1] = 1;						  /* command */
	sc2[2] = 0;						  /* reserved */
	sc2[3] = 3;						  /* address type */
	sc2[4] = (unsigned char) addrlen;	/* hostname length */
	memcpy (sc2 + 5, serverAddr, addrlen);
	*((unsigned short *) (sc2 + 5 + addrlen)) = htons (port);
	send (sok, sc2, packetlen, 0);
	free (sc2);

	/* consume all of the reply */
	if (recv (sok, buf, 4, 0) != 4)
		goto read_error;
	if (buf[0] != 5 || buf[1] != 0)
	{
		if (buf[1] == 2)
			snprintf (buf, sizeof (buf), "SOCKS\tProxy refused to connect to host (not allowed).\n");
		else
			snprintf (buf, sizeof (buf), "SOCKS\tProxy failed to connect to host (error %d).\n", buf[1]);
		proxy_error (print_fd, buf);
		return 1;
	}
	if (buf[3] == 1)	/* IPV4 32bit address */
	{
		if (recv (sok, buf, 6, 0) != 6)
			goto read_error;
	} else if (buf[3] == 4)	/* IPV6 128bit address */
	{
		if (recv (sok, buf, 18, 0) != 18)
			goto read_error;
	} else if (buf[3] == 3)	/* string, 1st byte is size */
	{
		if (recv (sok, buf, 1, 0) != 1)	/* read the string size */
			goto read_error;
		packetlen = buf[0] + 2;	/* can't exceed 260 */
		if (recv (sok, buf, packetlen, 0) != packetlen)
			goto read_error;
	}

	return 0;	/* success */

read_error:
	proxy_error (print_fd, "SOCKS\tRead error from server.\n");
	return 1;
}
