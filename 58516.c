static int nfsd_startup_net(int nrservs, struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	int ret;

	if (nn->nfsd_net_up)
		return 0;

	ret = nfsd_startup_generic(nrservs);
	if (ret)
		return ret;
	ret = nfsd_init_socks(net);
	if (ret)
		goto out_socks;

	if (nfsd_needs_lockd() && !nn->lockd_up) {
		ret = lockd_up(net);
		if (ret)
			goto out_socks;
		nn->lockd_up = 1;
	}

	ret = nfs4_state_start_net(net);
	if (ret)
		goto out_lockd;

	nn->nfsd_net_up = true;
	return 0;

out_lockd:
	if (nn->lockd_up) {
		lockd_down(net);
		nn->lockd_up = 0;
	}
out_socks:
	nfsd_shutdown_generic();
	return ret;
}
