int main (int argc, char **argv) {
	int result;
	struct sockaddr_in si_me;
	struct sockaddr_in si_me_mndp;
	struct timeval timeout;
	struct mt_packet pdata;
	struct net_interface *interface;
	fd_set read_fds;
	int c,optval = 1;
	int print_help = 0;
	int foreground = 0;
	int interface_count = 0;

	setlocale(LC_ALL, "");
	bindtextdomain("mactelnet","/usr/share/locale");
	textdomain("mactelnet");

	while ((c = getopt(argc, argv, "fnvh?")) != -1) {
		switch (c) {
			case 'f':
				foreground = 1;
				break;

			case 'n':
				use_raw_socket = 1;
				break;

			case 'v':
				print_version();
				exit(0);
				break;

			case 'h':
			case '?':
				print_help = 1;
				break;

		}
	}

	if (print_help) {
		print_version();
		fprintf(stderr, _("Usage: %s [-f|-n|-h]\n"), argv[0]);

		if (print_help) {
			fprintf(stderr, _("\nParameters:\n"
			"  -f        Run process in foreground.\n"
			"  -n        Do not use broadcast packets. Just a tad less insecure.\n"
			"  -h        This help.\n"
			"\n"));
		}
		return 1;
	}

	if (geteuid() != 0) {
		fprintf(stderr, _("You need to have root privileges to use %s.\n"), argv[0]);
		return 1;
	}

	/* Try to read user file */
	read_userfile();

	/* Seed randomizer */
	srand(time(NULL));

	if (use_raw_socket) {
		/* Transmit raw packets with this socket */
		sockfd = net_init_raw_socket();
	}

	/* Receive regular udp packets with this socket */
	insockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (insockfd < 0) {
		perror("insockfd");
		return 1;
	}

	/* Set source port */
	sourceport = MT_MACTELNET_PORT;

	/* Listen address*/
	inet_pton(AF_INET, (char *)"0.0.0.0", &sourceip);

	/* Set up global info about the connection */
	inet_pton(AF_INET, (char *)"255.255.255.255", &destip);

	/* Initialize receiving socket on the device chosen */
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(sourceport);
	memcpy(&(si_me.sin_addr), &sourceip, IPV4_ALEN);

	setsockopt(insockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));

	/* Bind to udp port */
	if (bind(insockfd, (struct sockaddr *)&si_me, sizeof(si_me))==-1) {
		fprintf(stderr, _("Error binding to %s:%d, %s\n"), inet_ntoa(si_me.sin_addr), sourceport, strerror(errno));
		return 1;
	}

	/* TODO: Move socket initialization out of main() */

	/* Receive mndp udp packets with this socket */
	mndpsockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mndpsockfd < 0) {
		perror("mndpsockfd");
		return 1;
	}

	memset((char *)&si_me_mndp, 0, sizeof(si_me_mndp));
	si_me_mndp.sin_family = AF_INET;
	si_me_mndp.sin_port = htons(MT_MNDP_PORT);
	memcpy(&(si_me_mndp.sin_addr), &sourceip, IPV4_ALEN);

	setsockopt(mndpsockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));

	/* Bind to udp port */
	if (bind(mndpsockfd, (struct sockaddr *)&si_me_mndp, sizeof(si_me_mndp))==-1) {
		fprintf(stderr, _("MNDP: Error binding to %s:%d, %s\n"), inet_ntoa(si_me_mndp.sin_addr), MT_MNDP_PORT, strerror(errno));
	}

	openlog("mactelnetd", LOG_PID, LOG_DAEMON);
	syslog(LOG_NOTICE, _("Bound to %s:%d"), inet_ntoa(si_me.sin_addr), sourceport);

	/* Enumerate available interfaces */
	net_get_interfaces(&interfaces);

	setup_sockets();

	if (!foreground) {
		daemon(0, 0);
	}

	/* Handle zombies etc */
	signal(SIGCHLD,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP, sighup_handler);
	signal(SIGTERM, sigterm_handler);

	DL_FOREACH(interfaces, interface) {
		if (interface->has_mac) {
			interface_count++;
		}
	}

	if (interface_count == 0) {
		syslog(LOG_ERR, _("Unable to find any valid network interfaces\n"));
		exit(1);
	}

	while (1) {
		int reads;
		struct mt_connection *p;
		int maxfd=0;
		time_t now;

		/* Init select */
		FD_ZERO(&read_fds);
		FD_SET(insockfd, &read_fds);
		FD_SET(mndpsockfd, &read_fds);
		maxfd = insockfd > mndpsockfd ? insockfd : mndpsockfd;

		/* Add active connections to select queue */
		DL_FOREACH(connections_head, p) {
			if (p->state == STATE_ACTIVE && p->wait_for_ack == 0 && p->ptsfd > 0) {
				FD_SET(p->ptsfd, &read_fds);
				if (p->ptsfd > maxfd) {
					maxfd = p->ptsfd;
				}
			}
		}

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		/* Wait for data or timeout */
		reads = select(maxfd+1, &read_fds, NULL, NULL, &timeout);
		if (reads > 0) {
			/* Handle data from clients
			 TODO: Enable broadcast support (without raw sockets)
			 */
			if (FD_ISSET(insockfd, &read_fds)) {
				unsigned char buff[MT_PACKET_LEN];
				struct sockaddr_in saddress;
				unsigned int slen = sizeof(saddress);
				bzero(buff, MT_HEADER_LEN);

				result = recvfrom(insockfd, buff, sizeof(buff), 0, (struct sockaddr *)&saddress, &slen);
				handle_packet(buff, result, &saddress);
			}
			if (FD_ISSET(mndpsockfd, &read_fds)) {
				unsigned char buff[MT_PACKET_LEN];
				struct sockaddr_in saddress;
				unsigned int slen = sizeof(saddress);
				result = recvfrom(mndpsockfd, buff, sizeof(buff), 0, (struct sockaddr *)&saddress, &slen);

				/* Handle MNDP broadcast request, max 1 rps */
				if (result == 4 && time(NULL) - last_mndp_time > 0) {
					mndp_broadcast();
					time(&last_mndp_time);
				}
			}
			/* Handle data from terminal sessions */
			DL_FOREACH(connections_head, p) {
				/* Check if we have data ready in the pty buffer for the active session */
				if (p->state == STATE_ACTIVE && p->ptsfd > 0 && p->wait_for_ack == 0 && FD_ISSET(p->ptsfd, &read_fds)) {
					unsigned char keydata[1024];
					int datalen,plen;

					/* Read it */
					datalen = read(p->ptsfd, &keydata, sizeof(keydata));
					if (datalen > 0) {
						/* Send it */
						init_packet(&pdata, MT_PTYPE_DATA, p->dstmac, p->srcmac, p->seskey, p->outcounter);
						plen = add_control_packet(&pdata, MT_CPTYPE_PLAINDATA, &keydata, datalen);
						p->outcounter += plen;
						p->wait_for_ack = 1;
						result = send_udp(p, &pdata);
					} else {
						/* Shell exited */
						struct mt_connection tmp;
						init_packet(&pdata, MT_PTYPE_END, p->dstmac, p->srcmac, p->seskey, p->outcounter);
						send_udp(p, &pdata);
						if (p->username != NULL) {
							syslog(LOG_INFO, _("(%d) Connection to user %s closed."), p->seskey, p->username);
						} else {
							syslog(LOG_INFO, _("(%d) Connection closed."), p->seskey);
						}
						tmp.next = p->next;
						list_remove_connection(p);
						p = &tmp;
					}
				}
				else if (p->state == STATE_ACTIVE && p->ptsfd > 0 && p->wait_for_ack == 1 && FD_ISSET(p->ptsfd, &read_fds)) {
					printf(_("(%d) Waiting for ack\n"), p->seskey);
				}
			}
		/* Handle select() timeout */
		}
		time(&now);

		if (now - last_mndp_time > MT_MNDP_BROADCAST_INTERVAL) {
			pings = 0;
			mndp_broadcast();
			last_mndp_time = now;
		}
		if (connections_head != NULL) {
			struct mt_connection *p,tmp;
			DL_FOREACH(connections_head, p) {
				if (now - p->lastdata >= MT_CONNECTION_TIMEOUT) {
					syslog(LOG_INFO, _("(%d) Session timed out"), p->seskey);
					init_packet(&pdata, MT_PTYPE_DATA, p->dstmac, p->srcmac, p->seskey, p->outcounter);
					/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
					add_control_packet(&pdata, MT_CPTYPE_PLAINDATA, _("Timeout\r\n"), 9);
					send_udp(p, &pdata);
					init_packet(&pdata, MT_PTYPE_END, p->dstmac, p->srcmac, p->seskey, p->outcounter);
					send_udp(p, &pdata);

					tmp.next = p->next;
					list_remove_connection(p);
					p = &tmp;
				}
			}
		}
	}

	/* Never reached */
	return 0;
}
