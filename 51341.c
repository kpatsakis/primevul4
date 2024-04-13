static int send_special_udp(struct net_interface *interface, unsigned short port, const struct mt_packet *packet) {
	unsigned char dstmac[ETH_ALEN];

	if (use_raw_socket) {
		memset(dstmac, 0xff, ETH_ALEN);
		return net_send_udp(sockfd, interface, interface->mac_addr, dstmac, (const struct in_addr *)&interface->ipv4_addr, port, &destip, port, packet->data, packet->size);
	} else {
		/* Init SendTo struct */
		struct sockaddr_in socket_address;
		socket_address.sin_family = AF_INET;
		socket_address.sin_port = htons(port);
		socket_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);

		return sendto(interface->socketfd, packet->data, packet->size, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
	}
}
