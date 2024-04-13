static struct mt_connection *list_find_connection(unsigned short seskey, unsigned char *srcmac) {
	struct mt_connection *p;

	DL_FOREACH(connections_head, p) {
		if (p->seskey == seskey && memcmp(srcmac, p->srcmac, ETH_ALEN) == 0) {
			return p;
		}
	}

	return NULL;
}
