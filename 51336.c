static void handle_packet(unsigned char *data, int data_len, const struct sockaddr_in *address) {
	struct mt_mactelnet_hdr pkthdr;
	struct mt_connection *curconn = NULL;
	struct mt_packet pdata;
	struct net_interface *interface;

	/* Check for minimal size */
	if (data_len < MT_HEADER_LEN - 4) {
		return;
	}
	parse_packet(data, &pkthdr);

	/* Drop packets not belonging to us */
	if ((interface = find_socket(pkthdr.dstaddr)) < 0) {
		return;
	}

	switch (pkthdr.ptype) {

		case MT_PTYPE_PING:
			if (pings++ > MT_MAXPPS) {
				/* Don't want it to wrap around back to the valid range */
				pings--;
				break;
			}
			init_pongpacket(&pdata, (unsigned char *)&(pkthdr.dstaddr), (unsigned char *)&(pkthdr.srcaddr));
			add_packetdata(&pdata, pkthdr.data - 4, data_len - (MT_HEADER_LEN - 4));
			{
				if (index >= 0) {
					send_special_udp(interface, MT_MACTELNET_PORT, &pdata);
				}
			}
			break;

		case MT_PTYPE_SESSIONSTART:
			curconn = list_find_connection(pkthdr.seskey, (unsigned char *)&(pkthdr.srcaddr));
			if (curconn != NULL) {
				/* Ignore multiple session starts from the same sender, this can be same mac but different interface */
				break;
			}
			syslog(LOG_DEBUG, _("(%d) New connection from %s."), pkthdr.seskey, ether_ntoa((struct ether_addr*)&(pkthdr.srcaddr)));
			curconn = calloc(1, sizeof(struct mt_connection));
			curconn->seskey = pkthdr.seskey;
			curconn->lastdata = time(NULL);
			curconn->state = STATE_AUTH;
			curconn->interface = interface;
			strncpy(curconn->interface_name, interface->name, 254);
			curconn->interface_name[255] = '\0';
			memcpy(curconn->srcmac, pkthdr.srcaddr, ETH_ALEN);
			memcpy(curconn->srcip, &(address->sin_addr), IPV4_ALEN);
			curconn->srcport = htons(address->sin_port);
			memcpy(curconn->dstmac, pkthdr.dstaddr, ETH_ALEN);

			list_add_connection(curconn);

			init_packet(&pdata, MT_PTYPE_ACK, pkthdr.dstaddr, pkthdr.srcaddr, pkthdr.seskey, pkthdr.counter);
			send_udp(curconn, &pdata);
			break;

		case MT_PTYPE_END:
			curconn = list_find_connection(pkthdr.seskey, (unsigned char *)&(pkthdr.srcaddr));
			if (curconn == NULL) {
				break;
			}
			if (curconn->state != STATE_CLOSED) {
				init_packet(&pdata, MT_PTYPE_END, pkthdr.dstaddr, pkthdr.srcaddr, pkthdr.seskey, pkthdr.counter);
				send_udp(curconn, &pdata);
			}
			syslog(LOG_DEBUG, _("(%d) Connection closed."), curconn->seskey);
			list_remove_connection(curconn);
			return;

		case MT_PTYPE_ACK:
			curconn = list_find_connection(pkthdr.seskey, (unsigned char *)&(pkthdr.srcaddr));
			if (curconn == NULL) {
				break;
			}

			if (pkthdr.counter <= curconn->outcounter) {
				curconn->wait_for_ack = 0;
				curconn->lastack = pkthdr.counter;
			}

			if (time(0) - curconn->lastdata > 9 || pkthdr.counter == curconn->lastack) {
				init_packet(&pdata, MT_PTYPE_ACK, pkthdr.dstaddr, pkthdr.srcaddr, pkthdr.seskey, pkthdr.counter);
				send_udp(curconn, &pdata);
			}
			curconn->lastdata = time(NULL);
			return;

		case MT_PTYPE_DATA:
			curconn = list_find_connection(pkthdr.seskey, (unsigned char *)&(pkthdr.srcaddr));
			if (curconn == NULL) {
				break;
			}
			curconn->lastdata = time(NULL);

			/* now check the right size */
			if (data_len < MT_HEADER_LEN) {
				/* Ignore illegal packet */
				return;
			}

			/* ack the data packet */
			init_packet(&pdata, MT_PTYPE_ACK, pkthdr.dstaddr, pkthdr.srcaddr, pkthdr.seskey, pkthdr.counter + (data_len - MT_HEADER_LEN));
			send_udp(curconn, &pdata);

			/* Accept first packet, and all packets greater than incounter, and if counter has
			wrapped around. */
			if (curconn->incounter == 0 || pkthdr.counter > curconn->incounter || (curconn->incounter - pkthdr.counter) > 16777216) {
				curconn->incounter = pkthdr.counter;
			} else {
				/* Ignore double or old packets */
				return;
			}

			handle_data_packet(curconn, &pkthdr, data_len);
			break;
		default:
			if (curconn) {
				syslog(LOG_WARNING, _("(%d) Unhandeled packet type: %d"), curconn->seskey, pkthdr.ptype);
				init_packet(&pdata, MT_PTYPE_ACK, pkthdr.dstaddr, pkthdr.srcaddr, pkthdr.seskey, pkthdr.counter);
				send_udp(curconn, &pdata);
			}
		}
	if (0 && curconn != NULL) {
		printf("Packet, incounter %d, outcounter %d\n", curconn->incounter, curconn->outcounter);
	}
}
