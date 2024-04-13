void send_packet(const node_t *n, vpn_packet_t *packet) {
	node_t *via;

	if(n == myself) {
		if(overwrite_mac)
			 memcpy(packet->data, mymac.x, ETH_ALEN);
		devops.write(packet);
		return;
	}

	ifdebug(TRAFFIC) logger(LOG_ERR, "Sending packet of %d bytes to %s (%s)",
			   packet->len, n->name, n->hostname);

	if(!n->status.reachable) {
		ifdebug(TRAFFIC) logger(LOG_INFO, "Node %s (%s) is not reachable",
				   n->name, n->hostname);
		return;
	}

	via = (packet->priority == -1 || n->via == myself) ? n->nexthop : n->via;

	if(via != n)
		ifdebug(TRAFFIC) logger(LOG_INFO, "Sending packet to %s via %s (%s)",
			   n->name, via->name, n->via->hostname);

	if(packet->priority == -1 || ((myself->options | via->options) & OPTION_TCPONLY)) {
		if(!send_tcppacket(via->connection, packet))
			terminate_connection(via->connection, true);
	} else
		send_udppacket(via, packet);
}
