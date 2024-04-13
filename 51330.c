static int send_udp(struct mt_packet *packet, int retransmit) {
	int sent_bytes;

	/* Clear keepalive counter */
	keepalive_counter = 0;

	if (!use_raw_socket) {
		/* Init SendTo struct */
		struct sockaddr_in socket_address;
		socket_address.sin_family = AF_INET;
		socket_address.sin_port = htons(MT_MACTELNET_PORT);
		socket_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);

		sent_bytes = sendto(send_socket, packet->data, packet->size, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
	} else {
		sent_bytes = net_send_udp(sockfd, active_interface, srcmac, dstmac, &sourceip,  sourceport, &destip, MT_MACTELNET_PORT, packet->data, packet->size);
	}

	/* 
	 * Retransmit packet if no data is received within
	 * retransmit_intervals milliseconds.
	 */
	if (retransmit) {
		int i;

		for (i = 0; i < MAX_RETRANSMIT_INTERVALS; ++i) {
			fd_set read_fds;
			int reads;
			struct timeval timeout;
			int interval = retransmit_intervals[i] * 1000;

			/* Init select */
			FD_ZERO(&read_fds);
			FD_SET(insockfd, &read_fds);
			timeout.tv_sec = 0;
			timeout.tv_usec = interval;

			/* Wait for data or timeout */
			reads = select(insockfd + 1, &read_fds, NULL, NULL, &timeout);
			if (reads && FD_ISSET(insockfd, &read_fds)) {
				unsigned char buff[MT_PACKET_LEN];
				int result;

				bzero(buff, sizeof(buff));
				result = recvfrom(insockfd, buff, sizeof(buff), 0, 0, 0);

				/* Handle incoming packets, waiting for an ack */
				if (result > 0 && handle_packet(buff, result) == MT_PTYPE_ACK) {
					return sent_bytes;
				}
			}

			/* Retransmit */
			send_udp(packet, 0);
		}

		if (is_a_tty && terminal_mode) {
			reset_term();
		}

		fprintf(stderr, _("\nConnection timed out\n"));
		exit(1);
	}
	return sent_bytes;
}
