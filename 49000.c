static CLIENT* handle_export_name(uint32_t opt, int net, GArray* servers, uint32_t cflags) {
	uint32_t namelen;
	char* name;
	int i;

	if (read(net, &namelen, sizeof(namelen)) < 0) {
		err("Negotiation failed/7: %m");
		return NULL;
	}
	namelen = ntohl(namelen);
	name = malloc(namelen+1);
	name[namelen]=0;
	if (read(net, name, namelen) < 0) {
		err("Negotiation failed/8: %m");
		free(name);
		return NULL;
	}
	for(i=0; i<servers->len; i++) {
		SERVER* serve = &(g_array_index(servers, SERVER, i));
		if(!strcmp(serve->servename, name)) {
			CLIENT* client = g_new0(CLIENT, 1);
			client->server = serve;
			client->exportsize = OFFT_MAX;
			client->net = net;
			client->modern = TRUE;
			client->transactionlogfd = -1;
			client->clientfeats = cflags;
			free(name);
			return client;
		}
	}
	err("Negotiation failed/8a: Requested export not found");
	free(name);
	return NULL;
}
