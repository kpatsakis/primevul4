void send_mtu_probe(node_t *n) {
	vpn_packet_t packet;
	int len, i;
	int timeout = 1;
	
	n->mtuprobes++;
	n->mtuevent = NULL;

	if(!n->status.reachable || !n->status.validkey) {
		ifdebug(TRAFFIC) logger(LOG_INFO, "Trying to send MTU probe to unreachable or rekeying node %s (%s)", n->name, n->hostname);
		n->mtuprobes = 0;
		return;
	}

	if(n->mtuprobes > 32) {
		if(!n->minmtu) {
			n->mtuprobes = 31;
			timeout = pinginterval;
			goto end;
		}

		ifdebug(TRAFFIC) logger(LOG_INFO, "%s (%s) did not respond to UDP ping, restarting PMTU discovery", n->name, n->hostname);
		n->mtuprobes = 1;
		n->minmtu = 0;
		n->maxmtu = MTU;
	}

	if(n->mtuprobes >= 10 && n->mtuprobes < 32 && !n->minmtu) {
		ifdebug(TRAFFIC) logger(LOG_INFO, "No response to MTU probes from %s (%s)", n->name, n->hostname);
		n->mtuprobes = 31;
	}

	if(n->mtuprobes == 30 || (n->mtuprobes < 30 && n->minmtu >= n->maxmtu)) {
		if(n->minmtu > n->maxmtu)
			n->minmtu = n->maxmtu;
		else
			n->maxmtu = n->minmtu;
		n->mtu = n->minmtu;
		ifdebug(TRAFFIC) logger(LOG_INFO, "Fixing MTU of %s (%s) to %d after %d probes", n->name, n->hostname, n->mtu, n->mtuprobes);
		n->mtuprobes = 31;
	}

	if(n->mtuprobes == 31) {
		timeout = pinginterval;
		goto end;
	} else if(n->mtuprobes == 32) {
		timeout = pingtimeout;
	}

	for(i = 0; i < 4 + localdiscovery; i++) {
		if(i == 0) {
			if(n->mtuprobes < 30 || n->maxmtu + 8 >= MTU)
				continue;
			len = n->maxmtu + 8;
		} else if(n->maxmtu <= n->minmtu) {
			len = n->maxmtu;
		} else {
			len = n->minmtu + 1 + rand() % (n->maxmtu - n->minmtu);
		}

		if(len < 64)
			len = 64;
		
		memset(packet.data, 0, 14);
		RAND_pseudo_bytes(packet.data + 14, len - 14);
		packet.len = len;
		if(i >= 4 && n->mtuprobes <= 10)
			packet.priority = -1;
		else
			packet.priority = 0;

		ifdebug(TRAFFIC) logger(LOG_INFO, "Sending MTU probe length %d to %s (%s)", len, n->name, n->hostname);

		send_udppacket(n, &packet);
	}

end:
	n->mtuevent = new_event();
	n->mtuevent->handler = (event_handler_t)send_mtu_probe;
	n->mtuevent->data = n;
	n->mtuevent->time = now + timeout;
	event_add(n->mtuevent);
}
