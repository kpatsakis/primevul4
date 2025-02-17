void broadcast_packet(const node_t *from, vpn_packet_t *packet) {
	avl_node_t *node;
	connection_t *c;
	node_t *n;

	if(from != myself)
		send_packet(myself, packet);

	if(tunnelserver || broadcast_mode == BMODE_NONE)
		return;

	ifdebug(TRAFFIC) logger(LOG_INFO, "Broadcasting packet of %d bytes from %s (%s)",
			   packet->len, from->name, from->hostname);

	switch(broadcast_mode) {
		case BMODE_MST:
			for(node = connection_tree->head; node; node = node->next) {
				c = node->data;

				if(c->status.active && c->status.mst && c != from->nexthop->connection)
					send_packet(c->node, packet);
			}
			break;

		case BMODE_DIRECT:
			if(from != myself)
				break;

			for(node = node_udp_tree->head; node; node = node->next) {
				n = node->data;

				if(n->status.reachable && ((n->via == myself && n->nexthop == n) || n->via == n))
					send_packet(n, packet);
			}
			break;

		default:
			break;
	}
}
