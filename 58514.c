static void nfsd_shutdown_net(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	nfs4_state_shutdown_net(net);
	if (nn->lockd_up) {
		lockd_down(net);
		nn->lockd_up = 0;
	}
	nn->nfsd_net_up = false;
	nfsd_shutdown_generic();
}
