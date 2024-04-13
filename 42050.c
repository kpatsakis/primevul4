void mtu_probe_h(node_t *n, vpn_packet_t *packet, length_t len) {
	ifdebug(TRAFFIC) logger(LOG_INFO, "Got MTU probe length %d from %s (%s)", packet->len, n->name, n->hostname);

	if(!packet->data[0]) {
		packet->data[0] = 1;
		send_udppacket(n, packet);
	} else {
		if(n->mtuprobes > 30) {
			if (len == n->maxmtu + 8) {
				ifdebug(TRAFFIC) logger(LOG_INFO, "Increase in PMTU to %s (%s) detected, restarting PMTU discovery", n->name, n->hostname);
				n->maxmtu = MTU;
				n->mtuprobes = 10;
				return;
			}

			if(n->minmtu)
				n->mtuprobes = 30;
			else
				n->mtuprobes = 1;
		}

		if(len > n->maxmtu)
			len = n->maxmtu;
		if(n->minmtu < len)
			n->minmtu = len;
	}
}
