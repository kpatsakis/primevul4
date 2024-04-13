static int send_udp(const struct mt_connection *conn, const struct mt_packet *packet) {
	if (use_raw_socket) {
		return net_send_udp(sockfd, conn->interface, conn->dstmac, conn->srcmac, &sourceip, sourceport, &destip, conn->srcport, packet->data, packet->size);
	} else {
		/* Init SendTo struct */
		struct sockaddr_in socket_address;
		socket_address.sin_family = AF_INET;
		socket_address.sin_port = htons(conn->srcport);
		socket_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);

		return sendto(conn->interface->socketfd, packet->data, packet->size, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
	}
}
