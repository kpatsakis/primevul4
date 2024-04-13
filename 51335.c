static struct net_interface *find_socket(unsigned char *mac) {
	struct net_interface *interface;

	DL_FOREACH(interfaces, interface) {
		if (memcmp(mac, interface->mac_addr, ETH_ALEN) == 0) {
			return interface;
		}
	}
	return NULL;
}
