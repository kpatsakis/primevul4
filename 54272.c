traverse_socks (int print_fd, int sok, char *serverAddr, int port)
{
	struct sock_connect sc;
	unsigned char buf[256];

	sc.version = 4;
	sc.type = 1;
	sc.port = htons (port);
	sc.address = inet_addr (serverAddr);
	strncpy (sc.username, prefs.hex_irc_user_name, 9);

	send (sok, (char *) &sc, 8 + strlen (sc.username) + 1, 0);
	buf[1] = 0;
	recv (sok, buf, 10, 0);
	if (buf[1] == 90)
		return 0;

	snprintf (buf, sizeof (buf), "SOCKS\tServer reported error %d,%d.\n", buf[0], buf[1]);
	proxy_error (print_fd, buf);
	return 1;
}
