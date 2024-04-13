static void setup_sockets() {
	struct net_interface *interface;

	DL_FOREACH(interfaces, interface) {
		int optval = 1;
		struct sockaddr_in si_me;
		struct ether_addr *mac = (struct ether_addr *)&(interface->mac_addr);

		if (!interface->has_mac) {
			continue;
		}

		if (!use_raw_socket) {
			interface->socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (interface->socketfd < 0) {
				continue;
			}

			if (setsockopt(interface->socketfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (optval))==-1) {
				perror("SO_BROADCAST");
				continue;
			}

			setsockopt(interface->socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

			/* Initialize receiving socket on the device chosen */
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(MT_MACTELNET_PORT);
			memcpy(&(si_me.sin_addr.s_addr), interface->ipv4_addr, IPV4_ALEN);

			if (bind(interface->socketfd, (struct sockaddr *)&si_me, sizeof(si_me))==-1) {
				fprintf(stderr, _("Error binding to %s:%d, %s\n"), inet_ntoa(si_me.sin_addr), sourceport, strerror(errno));
				continue;
			}
		}

		syslog(LOG_NOTICE, _("Listening on %s for %s\n"), interface->name, ether_ntoa(mac));

	}
}
