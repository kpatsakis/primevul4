CLIENT* negotiate(int net, CLIENT *client, GArray* servers, int phase) {
	char zeros[128];
	uint64_t size_host;
	uint32_t flags = NBD_FLAG_HAS_FLAGS;
	uint16_t smallflags = 0;
	uint64_t magic;

	memset(zeros, '\0', sizeof(zeros));
	assert(((phase & NEG_INIT) && (phase & NEG_MODERN)) || client);
	if(phase & NEG_MODERN) {
		smallflags |= NBD_FLAG_FIXED_NEWSTYLE;
	}
	if(phase & NEG_INIT) {
		/* common */
		if (write(net, INIT_PASSWD, 8) < 0) {
			err_nonfatal("Negotiation failed/1: %m");
			if(client)
				exit(EXIT_FAILURE);
		}
		if(phase & NEG_MODERN) {
			/* modern */
			magic = htonll(opts_magic);
		} else {
			/* oldstyle */
			magic = htonll(cliserv_magic);
		}
		if (write(net, &magic, sizeof(magic)) < 0) {
			err_nonfatal("Negotiation failed/2: %m");
			if(phase & NEG_OLD)
				exit(EXIT_FAILURE);
		}
	}
	if ((phase & NEG_MODERN) && (phase & NEG_INIT)) {
		/* modern */
		uint32_t cflags;
		uint32_t opt;

		if(!servers)
			err("programmer error");
		smallflags = htons(smallflags);
		if (write(net, &smallflags, sizeof(uint16_t)) < 0)
			err_nonfatal("Negotiation failed/3: %m");
		if (read(net, &cflags, sizeof(cflags)) < 0)
			err_nonfatal("Negotiation failed/4: %m");
		cflags = htonl(cflags);
		do {
			if (read(net, &magic, sizeof(magic)) < 0)
				err_nonfatal("Negotiation failed/5: %m");
			magic = ntohll(magic);
			if(magic != opts_magic) {
				err_nonfatal("Negotiation failed/5a: magic mismatch");
				return NULL;
			}
			if (read(net, &opt, sizeof(opt)) < 0)
				err_nonfatal("Negotiation failed/6: %m");
			opt = ntohl(opt);
			switch(opt) {
			case NBD_OPT_EXPORT_NAME:
				return handle_export_name(opt, net, servers, cflags);
				break;
			case NBD_OPT_LIST:
				handle_list(opt, net, servers, cflags);
				break;
			case NBD_OPT_ABORT:
				break;
			default:
				send_reply(opt, net, NBD_REP_ERR_UNSUP, 0, NULL);
				break;
			}
		} while((opt != NBD_OPT_EXPORT_NAME) && (opt != NBD_OPT_ABORT));
		if(opt == NBD_OPT_ABORT) {
			err_nonfatal("Session terminated by client");
			return NULL;
		}
	}
	/* common */
	size_host = htonll((u64)(client->exportsize));
	if (write(net, &size_host, 8) < 0)
		err("Negotiation failed/9: %m");
	if (client->server->flags & F_READONLY)
		flags |= NBD_FLAG_READ_ONLY;
	if (client->server->flags & F_FLUSH)
		flags |= NBD_FLAG_SEND_FLUSH;
	if (client->server->flags & F_FUA)
		flags |= NBD_FLAG_SEND_FUA;
	if (client->server->flags & F_ROTATIONAL)
		flags |= NBD_FLAG_ROTATIONAL;
	if (client->server->flags & F_TRIM)
		flags |= NBD_FLAG_SEND_TRIM;
	if (phase & NEG_OLD) {
		/* oldstyle */
		flags = htonl(flags);
		if (write(client->net, &flags, 4) < 0)
			err("Negotiation failed/10: %m");
	} else {
		/* modern */
		smallflags = (uint16_t)(flags & ~((uint16_t)0));
		smallflags = htons(smallflags);
		if (write(client->net, &smallflags, sizeof(smallflags)) < 0) {
			err("Negotiation failed/11: %m");
		}
	}
	/* common */
	if (write(client->net, zeros, 124) < 0)
		err("Negotiation failed/12: %m");
	return NULL;
}
