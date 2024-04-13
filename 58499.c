int nfsd_create_serv(struct net *net)
{
	int error;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	WARN_ON(!mutex_is_locked(&nfsd_mutex));
	if (nn->nfsd_serv) {
		svc_get(nn->nfsd_serv);
		return 0;
	}
	if (nfsd_max_blksize == 0)
		nfsd_max_blksize = nfsd_get_default_max_blksize();
	nfsd_reset_versions();
	nn->nfsd_serv = svc_create_pooled(&nfsd_program, nfsd_max_blksize,
						&nfsd_thread_sv_ops);
	if (nn->nfsd_serv == NULL)
		return -ENOMEM;

	nn->nfsd_serv->sv_maxconn = nn->max_connections;
	error = svc_bind(nn->nfsd_serv, net);
	if (error < 0) {
		svc_destroy(nn->nfsd_serv);
		return error;
	}

	set_max_drc();
	/* check if the notifier is already set */
	if (atomic_inc_return(&nfsd_notifier_refcount) == 1) {
		register_inetaddr_notifier(&nfsd_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
		register_inet6addr_notifier(&nfsd_inet6addr_notifier);
#endif
	}
	do_gettimeofday(&nn->nfssvc_boot);		/* record boot time */
	return 0;
}
