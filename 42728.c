cifs_reconnect(struct TCP_Server_Info *server)
{
	int rc = 0;
	struct list_head *tmp, *tmp2;
	struct cifsSesInfo *ses;
	struct cifsTconInfo *tcon;
	struct mid_q_entry *mid_entry;

	spin_lock(&GlobalMid_Lock);
	if (server->tcpStatus == CifsExiting) {
		/* the demux thread will exit normally
		next time through the loop */
		spin_unlock(&GlobalMid_Lock);
		return rc;
	} else
		server->tcpStatus = CifsNeedReconnect;
	spin_unlock(&GlobalMid_Lock);
	server->maxBuf = 0;

	cFYI(1, "Reconnecting tcp session");

	/* before reconnecting the tcp session, mark the smb session (uid)
		and the tid bad so they are not used until reconnected */
	read_lock(&cifs_tcp_ses_lock);
	list_for_each(tmp, &server->smb_ses_list) {
		ses = list_entry(tmp, struct cifsSesInfo, smb_ses_list);
		ses->need_reconnect = true;
		ses->ipc_tid = 0;
		list_for_each(tmp2, &ses->tcon_list) {
			tcon = list_entry(tmp2, struct cifsTconInfo, tcon_list);
			tcon->need_reconnect = true;
		}
	}
	read_unlock(&cifs_tcp_ses_lock);
	/* do not want to be sending data on a socket we are freeing */
	mutex_lock(&server->srv_mutex);
	if (server->ssocket) {
		cFYI(1, "State: 0x%x Flags: 0x%lx", server->ssocket->state,
			server->ssocket->flags);
		kernel_sock_shutdown(server->ssocket, SHUT_WR);
		cFYI(1, "Post shutdown state: 0x%x Flags: 0x%lx",
			server->ssocket->state,
			server->ssocket->flags);
		sock_release(server->ssocket);
		server->ssocket = NULL;
	}

	spin_lock(&GlobalMid_Lock);
	list_for_each(tmp, &server->pending_mid_q) {
		mid_entry = list_entry(tmp, struct
					mid_q_entry,
					qhead);
		if (mid_entry->midState == MID_REQUEST_SUBMITTED) {
				/* Mark other intransit requests as needing
				   retry so we do not immediately mark the
				   session bad again (ie after we reconnect
				   below) as they timeout too */
			mid_entry->midState = MID_RETRY_NEEDED;
		}
	}
	spin_unlock(&GlobalMid_Lock);
	mutex_unlock(&server->srv_mutex);

	while ((server->tcpStatus != CifsExiting) &&
	       (server->tcpStatus != CifsGood)) {
		try_to_freeze();
		if (server->addr.sockAddr6.sin6_family == AF_INET6)
			rc = ipv6_connect(server);
		else
			rc = ipv4_connect(server);
		if (rc) {
			cFYI(1, "reconnect error %d", rc);
			msleep(3000);
		} else {
			atomic_inc(&tcpSesReconnectCount);
			spin_lock(&GlobalMid_Lock);
			if (server->tcpStatus != CifsExiting)
				server->tcpStatus = CifsGood;
			server->sequence_number = 0;
			spin_unlock(&GlobalMid_Lock);
	/*		atomic_set(&server->inFlight,0);*/
			wake_up(&server->response_q);
		}
	}
	return rc;
}
