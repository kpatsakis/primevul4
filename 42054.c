static void send_udppacket(node_t *n, vpn_packet_t *origpkt) {
	vpn_packet_t pkt1, pkt2;
	vpn_packet_t *pkt[] = { &pkt1, &pkt2, &pkt1, &pkt2 };
	vpn_packet_t *inpkt = origpkt;
	int nextpkt = 0;
	vpn_packet_t *outpkt;
	int origlen;
	int outlen, outpad;
#if defined(SOL_IP) && defined(IP_TOS)
	static int priority = 0;
#endif
	int origpriority;

	if(!n->status.reachable) {
		ifdebug(TRAFFIC) logger(LOG_INFO, "Trying to send UDP packet to unreachable node %s (%s)", n->name, n->hostname);
		return;
	}

	/* Make sure we have a valid key */

	if(!n->status.validkey) {
		ifdebug(TRAFFIC) logger(LOG_INFO,
				   "No valid key known yet for %s (%s), forwarding via TCP",
				   n->name, n->hostname);

		if(n->last_req_key + 10 <= now) {
			send_req_key(n);
			n->last_req_key = now;
		}

		send_tcppacket(n->nexthop->connection, origpkt);

		return;
	}

	if(n->options & OPTION_PMTU_DISCOVERY && inpkt->len > n->minmtu && (inpkt->data[12] | inpkt->data[13])) {
		ifdebug(TRAFFIC) logger(LOG_INFO,
				"Packet for %s (%s) larger than minimum MTU, forwarding via %s",
				n->name, n->hostname, n != n->nexthop ? n->nexthop->name : "TCP");

		if(n != n->nexthop)
			send_packet(n->nexthop, origpkt);
		else
			send_tcppacket(n->nexthop->connection, origpkt);

		return;
	}

	origlen = inpkt->len;
	origpriority = inpkt->priority;

	/* Compress the packet */

	if(n->outcompression) {
		outpkt = pkt[nextpkt++];

		if((outpkt->len = compress_packet(outpkt->data, inpkt->data, inpkt->len, n->outcompression)) < 0) {
			ifdebug(TRAFFIC) logger(LOG_ERR, "Error while compressing packet to %s (%s)",
				   n->name, n->hostname);
			return;
		}

		inpkt = outpkt;
	}

	/* Add sequence number */

	inpkt->seqno = htonl(++(n->sent_seqno));
	inpkt->len += sizeof(inpkt->seqno);

	/* Encrypt the packet */

	if(n->outcipher) {
		outpkt = pkt[nextpkt++];

		if(!EVP_EncryptInit_ex(&n->outctx, NULL, NULL, NULL, NULL)
				|| !EVP_EncryptUpdate(&n->outctx, (unsigned char *) &outpkt->seqno, &outlen,
					(unsigned char *) &inpkt->seqno, inpkt->len)
				|| !EVP_EncryptFinal_ex(&n->outctx, (unsigned char *) &outpkt->seqno + outlen, &outpad)) {
			ifdebug(TRAFFIC) logger(LOG_ERR, "Error while encrypting packet to %s (%s): %s",
						n->name, n->hostname, ERR_error_string(ERR_get_error(), NULL));
			goto end;
		}

		outpkt->len = outlen + outpad;
		inpkt = outpkt;
	}

	/* Add the message authentication code */

	if(n->outdigest && n->outmaclength) {
		HMAC(n->outdigest, n->outkey, n->outkeylength, (unsigned char *) &inpkt->seqno,
			 inpkt->len, (unsigned char *) &inpkt->seqno + inpkt->len, NULL);
		inpkt->len += n->outmaclength;
	}

	/* Determine which socket we have to use */

	if(n->address.sa.sa_family != listen_socket[n->sock].sa.sa.sa_family) {
		for(int sock = 0; sock < listen_sockets; sock++) {
			if(n->address.sa.sa_family == listen_socket[sock].sa.sa.sa_family) {
				n->sock = sock;
				break;
			}
		}
	}

	/* Send the packet */

	struct sockaddr *sa;
	socklen_t sl;
	int sock;
	sockaddr_t broadcast;

	/* Overloaded use of priority field: -1 means local broadcast */

	if(origpriority == -1 && n->prevedge) {
		sock = rand() % listen_sockets;
		memset(&broadcast, 0, sizeof broadcast);
		if(listen_socket[sock].sa.sa.sa_family == AF_INET6) {
			broadcast.in6.sin6_family = AF_INET6;
			broadcast.in6.sin6_addr.s6_addr[0x0] = 0xff;
			broadcast.in6.sin6_addr.s6_addr[0x1] = 0x02;
			broadcast.in6.sin6_addr.s6_addr[0xf] = 0x01;
			broadcast.in6.sin6_port = n->prevedge->address.in.sin_port;
			broadcast.in6.sin6_scope_id = listen_socket[sock].sa.in6.sin6_scope_id;
		} else {
			broadcast.in.sin_family = AF_INET;
			broadcast.in.sin_addr.s_addr = -1;
			broadcast.in.sin_port = n->prevedge->address.in.sin_port;
		}
		sa = &broadcast.sa;
		sl = SALEN(broadcast.sa);
	} else {
		if(origpriority == -1)
			origpriority = 0;

		sa = &(n->address.sa);
		sl = SALEN(n->address.sa);
		sock = n->sock;
	}

#if defined(SOL_IP) && defined(IP_TOS)
	if(priorityinheritance && origpriority != priority
	   && listen_socket[n->sock].sa.sa.sa_family == AF_INET) {
		priority = origpriority;
		ifdebug(TRAFFIC) logger(LOG_DEBUG, "Setting outgoing packet priority to %d", priority);
		if(setsockopt(listen_socket[n->sock].udp, SOL_IP, IP_TOS, &priority, sizeof(priority)))	/* SO_PRIORITY doesn't seem to work */
			logger(LOG_ERR, "System call `%s' failed: %s", "setsockopt", strerror(errno));
	}
#endif

	if(sendto(listen_socket[sock].udp, (char *) &inpkt->seqno, inpkt->len, 0, sa, sl) < 0 && !sockwouldblock(sockerrno)) {
		if(sockmsgsize(sockerrno)) {
			if(n->maxmtu >= origlen)
				n->maxmtu = origlen - 1;
			if(n->mtu >= origlen)
				n->mtu = origlen - 1;
		} else
			ifdebug(TRAFFIC) logger(LOG_WARNING, "Error sending packet to %s (%s): %s", n->name, n->hostname, sockstrerror(sockerrno));
	}

end:
	origpkt->len = origlen;
}
