ipv4_connect(struct TCP_Server_Info *server)
{
	int rc = 0;
	int val;
	bool connected = false;
	__be16 orig_port = 0;
	struct socket *socket = server->ssocket;

	if (socket == NULL) {
		rc = sock_create_kern(PF_INET, SOCK_STREAM,
				      IPPROTO_TCP, &socket);
		if (rc < 0) {
			cERROR(1, "Error %d creating socket", rc);
			return rc;
		}

		/* BB other socket options to set KEEPALIVE, NODELAY? */
		cFYI(1, "Socket created");
		server->ssocket = socket;
		socket->sk->sk_allocation = GFP_NOFS;
		cifs_reclassify_socket4(socket);
	}

	/* user overrode default port */
	if (server->addr.sockAddr.sin_port) {
		rc = socket->ops->connect(socket, (struct sockaddr *)
					  &server->addr.sockAddr,
					  sizeof(struct sockaddr_in), 0);
		if (rc >= 0)
			connected = true;
	}

	if (!connected) {
		/* save original port so we can retry user specified port
			later if fall back ports fail this time  */
		orig_port = server->addr.sockAddr.sin_port;

		/* do not retry on the same port we just failed on */
		if (server->addr.sockAddr.sin_port != htons(CIFS_PORT)) {
			server->addr.sockAddr.sin_port = htons(CIFS_PORT);
			rc = socket->ops->connect(socket,
						(struct sockaddr *)
						&server->addr.sockAddr,
						sizeof(struct sockaddr_in), 0);
			if (rc >= 0)
				connected = true;
		}
	}
	if (!connected) {
		server->addr.sockAddr.sin_port = htons(RFC1001_PORT);
		rc = socket->ops->connect(socket, (struct sockaddr *)
					      &server->addr.sockAddr,
					      sizeof(struct sockaddr_in), 0);
		if (rc >= 0)
			connected = true;
	}

	/* give up here - unless we want to retry on different
		protocol families some day */
	if (!connected) {
		if (orig_port)
			server->addr.sockAddr.sin_port = orig_port;
		cFYI(1, "Error %d connecting to server via ipv4", rc);
		sock_release(socket);
		server->ssocket = NULL;
		return rc;
	}


	/*
	 * Eventually check for other socket options to change from
	 *  the default. sock_setsockopt not used because it expects
	 *  user space buffer
	 */
	socket->sk->sk_rcvtimeo = 7 * HZ;
	socket->sk->sk_sndtimeo = 5 * HZ;

	/* make the bufsizes depend on wsize/rsize and max requests */
	if (server->noautotune) {
		if (socket->sk->sk_sndbuf < (200 * 1024))
			socket->sk->sk_sndbuf = 200 * 1024;
		if (socket->sk->sk_rcvbuf < (140 * 1024))
			socket->sk->sk_rcvbuf = 140 * 1024;
	}

	if (server->tcp_nodelay) {
		val = 1;
		rc = kernel_setsockopt(socket, SOL_TCP, TCP_NODELAY,
				(char *)&val, sizeof(val));
		if (rc)
			cFYI(1, "set TCP_NODELAY socket option error %d", rc);
	}

	 cFYI(1, "sndbuf %d rcvbuf %d rcvtimeo 0x%lx",
		 socket->sk->sk_sndbuf,
		 socket->sk->sk_rcvbuf, socket->sk->sk_rcvtimeo);

	/* send RFC1001 sessinit */
	if (server->addr.sockAddr.sin_port == htons(RFC1001_PORT)) {
		/* some servers require RFC1001 sessinit before sending
		negprot - BB check reconnection in case where second
		sessinit is sent but no second negprot */
		struct rfc1002_session_packet *ses_init_buf;
		struct smb_hdr *smb_buf;
		ses_init_buf = kzalloc(sizeof(struct rfc1002_session_packet),
				       GFP_KERNEL);
		if (ses_init_buf) {
			ses_init_buf->trailer.session_req.called_len = 32;
			if (server->server_RFC1001_name &&
			    server->server_RFC1001_name[0] != 0)
				rfc1002mangle(ses_init_buf->trailer.
						session_req.called_name,
					      server->server_RFC1001_name,
					      RFC1001_NAME_LEN_WITH_NULL);
			else
				rfc1002mangle(ses_init_buf->trailer.
						session_req.called_name,
					      DEFAULT_CIFS_CALLED_NAME,
					      RFC1001_NAME_LEN_WITH_NULL);

			ses_init_buf->trailer.session_req.calling_len = 32;

			/* calling name ends in null (byte 16) from old smb
			convention. */
			if (server->workstation_RFC1001_name &&
			    server->workstation_RFC1001_name[0] != 0)
				rfc1002mangle(ses_init_buf->trailer.
						session_req.calling_name,
					      server->workstation_RFC1001_name,
					      RFC1001_NAME_LEN_WITH_NULL);
			else
				rfc1002mangle(ses_init_buf->trailer.
						session_req.calling_name,
					      "LINUX_CIFS_CLNT",
					      RFC1001_NAME_LEN_WITH_NULL);

			ses_init_buf->trailer.session_req.scope1 = 0;
			ses_init_buf->trailer.session_req.scope2 = 0;
			smb_buf = (struct smb_hdr *)ses_init_buf;
			/* sizeof RFC1002_SESSION_REQUEST with no scope */
			smb_buf->smb_buf_length = 0x81000044;
			rc = smb_send(server, smb_buf, 0x44);
			kfree(ses_init_buf);
			msleep(1); /* RFC1001 layer in at least one server
				      requires very short break before negprot
				      presumably because not expecting negprot
				      to follow so fast.  This is a simple
				      solution that works without
				      complicating the code and causes no
				      significant slowing down on mount
				      for everyone else */
		}
		/* else the negprot may still work without this
		even though malloc failed */

	}

	return rc;
}
