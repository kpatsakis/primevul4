int mainloop(CLIENT *client) {
	struct nbd_request request;
	struct nbd_reply reply;
	gboolean go_on=TRUE;
#ifdef DODBG
	int i = 0;
#endif
	negotiate(client->net, client, NULL, client->modern ? NEG_MODERN : (NEG_OLD | NEG_INIT));
	DEBUG("Entering request loop!\n");
	reply.magic = htonl(NBD_REPLY_MAGIC);
	reply.error = 0;
	while (go_on) {
		char buf[BUFSIZE];
		char* p;
		size_t len;
		size_t currlen;
		size_t writelen;
		uint16_t command;
#ifdef DODBG
		i++;
		printf("%d: ", i);
#endif
		readit(client->net, &request, sizeof(request));
		if (client->transactionlogfd != -1)
			writeit(client->transactionlogfd, &request, sizeof(request));

		request.from = ntohll(request.from);
		request.type = ntohl(request.type);
		command = request.type & NBD_CMD_MASK_COMMAND;
		len = ntohl(request.len);

		DEBUG("%s from %llu (%llu) len %u, ", getcommandname(command),
				(unsigned long long)request.from,
				(unsigned long long)request.from / 512, len);

		if (request.magic != htonl(NBD_REQUEST_MAGIC))
			err("Not enough magic.");

		memcpy(reply.handle, request.handle, sizeof(reply.handle));

		if ((command==NBD_CMD_WRITE) || (command==NBD_CMD_READ)) {
			if (request.from + len < request.from) { // 64 bit overflow!!
				DEBUG("[Number too large!]");
				ERROR(client, reply, EINVAL);
				continue;
			}

			if (((off_t)request.from + len) > client->exportsize) {
				DEBUG("[RANGE!]");
				ERROR(client, reply, EINVAL);
				continue;
			}

			currlen = len;
			if (currlen > BUFSIZE - sizeof(struct nbd_reply)) {
				currlen = BUFSIZE - sizeof(struct nbd_reply);
				if(!logged_oversized) {
					msg(LOG_DEBUG, "oversized request (this is not a problem)");
					logged_oversized = true;
				}
			}
		}

		switch (command) {

		case NBD_CMD_DISC:
			msg(LOG_INFO, "Disconnect request received.");
                	if (client->server->flags & F_COPYONWRITE) { 
				if (client->difmap) g_free(client->difmap) ;
                		close(client->difffile);
				unlink(client->difffilename);
				free(client->difffilename);
			}
			go_on=FALSE;
			continue;

		case NBD_CMD_WRITE:
			DEBUG("wr: net->buf, ");
			while(len > 0) {
				readit(client->net, buf, currlen);
				DEBUG("buf->exp, ");
				if ((client->server->flags & F_READONLY) ||
				    (client->server->flags & F_AUTOREADONLY)) {
					DEBUG("[WRITE to READONLY!]");
					ERROR(client, reply, EPERM);
					consume(client->net, buf, len-currlen, BUFSIZE);
					continue;
				}
				if (expwrite(request.from, buf, currlen, client,
					     request.type & NBD_CMD_FLAG_FUA)) {
					DEBUG("Write failed: %m" );
					ERROR(client, reply, errno);
					consume(client->net, buf, len-currlen, BUFSIZE);
					continue;
				}
				len -= currlen;
				request.from += currlen;
				currlen = (len < BUFSIZE) ? len : BUFSIZE;
			}
			SEND(client->net, reply);
			DEBUG("OK!\n");
			continue;

		case NBD_CMD_FLUSH:
			DEBUG("fl: ");
			if (expflush(client)) {
				DEBUG("Flush failed: %m");
				ERROR(client, reply, errno);
				continue;
			}
			SEND(client->net, reply);
			DEBUG("OK!\n");
			continue;

		case NBD_CMD_READ:
			DEBUG("exp->buf, ");
			if (client->transactionlogfd != -1)
				writeit(client->transactionlogfd, &reply, sizeof(reply));
			writeit(client->net, &reply, sizeof(reply));
			p = buf;
			writelen = currlen;
			while(len > 0) {
				if (expread(request.from, p, currlen, client)) {
					DEBUG("Read failed: %m");
					ERROR(client, reply, errno);
					continue;
				}
				
				DEBUG("buf->net, ");
				writeit(client->net, buf, writelen);
				len -= currlen;
				request.from += currlen;
				currlen = (len < BUFSIZE) ? len : BUFSIZE;
				p = buf;
				writelen = currlen;
			}
			DEBUG("OK!\n");
			continue;

		case NBD_CMD_TRIM:
			/* The kernel module sets discard_zeroes_data == 0,
			 * so it is okay to do nothing.  */
			if (exptrim(&request, client)) {
				DEBUG("Trim failed: %m");
				ERROR(client, reply, errno);
				continue;
			}
			SEND(client->net, reply);
			continue;

		default:
			DEBUG ("Ignoring unknown command\n");
			continue;
		}
	}
	return 0;
}
