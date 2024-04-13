static node_t *try_harder(const sockaddr_t *from, const vpn_packet_t *pkt) {
	avl_node_t *node;
	edge_t *e;
	node_t *n = NULL;
	bool hard = false;
	static time_t last_hard_try = 0;

	for(node = edge_weight_tree->head; node; node = node->next) {
		e = node->data;

		if(e->to == myself)
			continue;

		if(sockaddrcmp_noport(from, &e->address)) {
			if(last_hard_try == now)
				continue;
			hard = true;
		}

		if(!try_mac(e->to, pkt))
			continue;

		n = e->to;
		break;
	}

	if(hard)
		last_hard_try = now;

	last_hard_try = now;
	return n;
}
