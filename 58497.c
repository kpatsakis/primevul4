nfsd(void *vrqstp)
{
	struct svc_rqst *rqstp = (struct svc_rqst *) vrqstp;
	struct svc_xprt *perm_sock = list_entry(rqstp->rq_server->sv_permsocks.next, typeof(struct svc_xprt), xpt_list);
	struct net *net = perm_sock->xpt_net;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	int err;

	/* Lock module and set up kernel thread */
	mutex_lock(&nfsd_mutex);

	/* At this point, the thread shares current->fs
	 * with the init process. We need to create files with the
	 * umask as defined by the client instead of init's umask. */
	if (unshare_fs_struct() < 0) {
		printk("Unable to start nfsd thread: out of memory\n");
		goto out;
	}

	current->fs->umask = 0;

	/*
	 * thread is spawned with all signals set to SIG_IGN, re-enable
	 * the ones that will bring down the thread
	 */
	allow_signal(SIGKILL);
	allow_signal(SIGHUP);
	allow_signal(SIGINT);
	allow_signal(SIGQUIT);

	nfsdstats.th_cnt++;
	mutex_unlock(&nfsd_mutex);

	set_freezable();

	/*
	 * The main request loop
	 */
	for (;;) {
		/* Update sv_maxconn if it has changed */
		rqstp->rq_server->sv_maxconn = nn->max_connections;

		/*
		 * Find a socket with data available and call its
		 * recvfrom routine.
		 */
		while ((err = svc_recv(rqstp, 60*60*HZ)) == -EAGAIN)
			;
		if (err == -EINTR)
			break;
		validate_process_creds();
		svc_process(rqstp);
		validate_process_creds();
	}

	/* Clear signals before calling svc_exit_thread() */
	flush_signals(current);

	mutex_lock(&nfsd_mutex);
	nfsdstats.th_cnt --;

out:
	rqstp->rq_server = NULL;

	/* Release the thread */
	svc_exit_thread(rqstp);

	nfsd_destroy(net);

	/* Release module */
	mutex_unlock(&nfsd_mutex);
	module_put_and_exit(0);
	return 0;
}
