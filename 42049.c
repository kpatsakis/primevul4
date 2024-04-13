void handle_incoming_vpn_data(int sock) {
	vpn_packet_t pkt;
	char *hostname;
	sockaddr_t from;
	socklen_t fromlen = sizeof(from);
	node_t *n;

	pkt.len = recvfrom(listen_socket[sock].udp, (char *) &pkt.seqno, MAXSIZE, 0, &from.sa, &fromlen);

	if(pkt.len < 0) {
		if(!sockwouldblock(sockerrno))
			logger(LOG_ERR, "Receiving packet failed: %s", sockstrerror(sockerrno));
		return;
	}

	sockaddrunmap(&from);		/* Some braindead IPv6 implementations do stupid things. */

	n = lookup_node_udp(&from);

	if(!n) {
		n = try_harder(&from, &pkt);
		if(n)
			update_node_udp(n, &from);
		else ifdebug(PROTOCOL) {
			hostname = sockaddr2hostname(&from);
			logger(LOG_WARNING, "Received UDP packet from unknown source %s", hostname);
			free(hostname);
			return;
		}
		else
			return;
	}

	n->sock = sock;

	receive_udppacket(n, &pkt);
}
