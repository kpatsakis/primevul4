int exptrim(struct nbd_request* req, CLIENT* client) {
#if HAVE_FALLOC_PH
	FILE_INFO prev = g_array_index(client->export, FILE_INFO, 0);
	FILE_INFO cur = prev;
	int i = 1;
	/* We're running on a system that supports the
	 * FALLOC_FL_PUNCH_HOLE option to re-sparsify a file */
	do {
		if(i<client->export->len) {
			cur = g_array_index(client->export, FILE_INFO, i);
		}
		if(prev.startoff <= req->from) {
			off_t curoff = req->from - prev.startoff;
			off_t curlen = cur.startoff - prev.startoff - curoff;
			fallocate(prev.fhandle, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, curoff, curlen);
		}
		prev = cur;
	} while(i < client->export->len && cur.startoff < (req->from + req->len));
	DEBUG("Performed TRIM request from %llu to %llu", (unsigned long long) req->from, (unsigned long long) req->len);
#else
	DEBUG("Ignoring TRIM request (not supported on current platform");
#endif
	return 0;
}
