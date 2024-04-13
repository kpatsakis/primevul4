cifs_get_tcp_session(struct smb_vol *volume_info)
{
	struct TCP_Server_Info *tcp_ses = NULL;
	struct sockaddr_storage addr;
	struct sockaddr_in *sin_server = (struct sockaddr_in *) &addr;
	struct sockaddr_in6 *sin_server6 = (struct sockaddr_in6 *) &addr;
	int rc;

	memset(&addr, 0, sizeof(struct sockaddr_storage));

	cFYI(1, "UNC: %s ip: %s", volume_info->UNC, volume_info->UNCip);

	if (volume_info->UNCip && volume_info->UNC) {
		rc = cifs_fill_sockaddr((struct sockaddr *)&addr,
					volume_info->UNCip,
					volume_info->port);
		if (!rc) {
			/* we failed translating address */
			rc = -EINVAL;
			goto out_err;
		}
	} else if (volume_info->UNCip) {
		/* BB using ip addr as tcp_ses name to connect to the
		   DFS root below */
		cERROR(1, "Connecting to DFS root not implemented yet");
		rc = -EINVAL;
		goto out_err;
	} else /* which tcp_sess DFS root would we conect to */ {
		cERROR(1, "CIFS mount error: No UNC path (e.g. -o "
			"unc=//192.168.1.100/public) specified");
		rc = -EINVAL;
		goto out_err;
	}

	/* see if we already have a matching tcp_ses */
	tcp_ses = cifs_find_tcp_session((struct sockaddr *)&addr, volume_info);
	if (tcp_ses)
		return tcp_ses;

	tcp_ses = kzalloc(sizeof(struct TCP_Server_Info), GFP_KERNEL);
	if (!tcp_ses) {
		rc = -ENOMEM;
		goto out_err;
	}

	tcp_ses->hostname = extract_hostname(volume_info->UNC);
	if (IS_ERR(tcp_ses->hostname)) {
		rc = PTR_ERR(tcp_ses->hostname);
		goto out_err;
	}

	tcp_ses->noblocksnd = volume_info->noblocksnd;
	tcp_ses->noautotune = volume_info->noautotune;
	tcp_ses->tcp_nodelay = volume_info->sockopt_tcp_nodelay;
	atomic_set(&tcp_ses->inFlight, 0);
	init_waitqueue_head(&tcp_ses->response_q);
	init_waitqueue_head(&tcp_ses->request_q);
	INIT_LIST_HEAD(&tcp_ses->pending_mid_q);
	mutex_init(&tcp_ses->srv_mutex);
	memcpy(tcp_ses->workstation_RFC1001_name,
		volume_info->source_rfc1001_name, RFC1001_NAME_LEN_WITH_NULL);
	memcpy(tcp_ses->server_RFC1001_name,
		volume_info->target_rfc1001_name, RFC1001_NAME_LEN_WITH_NULL);
	tcp_ses->sequence_number = 0;
	INIT_LIST_HEAD(&tcp_ses->tcp_ses_list);
	INIT_LIST_HEAD(&tcp_ses->smb_ses_list);

	/*
	 * at this point we are the only ones with the pointer
	 * to the struct since the kernel thread not created yet
	 * no need to spinlock this init of tcpStatus or srv_count
	 */
	tcp_ses->tcpStatus = CifsNew;
	++tcp_ses->srv_count;

	if (addr.ss_family == AF_INET6) {
		cFYI(1, "attempting ipv6 connect");
		/* BB should we allow ipv6 on port 139? */
		/* other OS never observed in Wild doing 139 with v6 */
		memcpy(&tcp_ses->addr.sockAddr6, sin_server6,
			sizeof(struct sockaddr_in6));
		rc = ipv6_connect(tcp_ses);
	} else {
		memcpy(&tcp_ses->addr.sockAddr, sin_server,
			sizeof(struct sockaddr_in));
		rc = ipv4_connect(tcp_ses);
	}
	if (rc < 0) {
		cERROR(1, "Error connecting to socket. Aborting operation");
		goto out_err;
	}

	/*
	 * since we're in a cifs function already, we know that
	 * this will succeed. No need for try_module_get().
	 */
	__module_get(THIS_MODULE);
	tcp_ses->tsk = kthread_run((void *)(void *)cifs_demultiplex_thread,
				  tcp_ses, "cifsd");
	if (IS_ERR(tcp_ses->tsk)) {
		rc = PTR_ERR(tcp_ses->tsk);
		cERROR(1, "error %d create cifsd thread", rc);
		module_put(THIS_MODULE);
		goto out_err;
	}

	/* thread spawned, put it on the list */
	write_lock(&cifs_tcp_ses_lock);
	list_add(&tcp_ses->tcp_ses_list, &cifs_tcp_ses_list);
	write_unlock(&cifs_tcp_ses_lock);

	cifs_fscache_get_client_cookie(tcp_ses);

	return tcp_ses;

out_err:
	if (tcp_ses) {
		if (!IS_ERR(tcp_ses->hostname))
			kfree(tcp_ses->hostname);
		if (tcp_ses->ssocket)
			sock_release(tcp_ses->ssocket);
		kfree(tcp_ses);
	}
	return ERR_PTR(rc);
 }
