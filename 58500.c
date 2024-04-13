void nfsd_destroy(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	int destroy = (nn->nfsd_serv->sv_nrthreads == 1);

	if (destroy)
		svc_shutdown_net(nn->nfsd_serv, net);
	svc_destroy(nn->nfsd_serv);
	if (destroy)
		nn->nfsd_serv = NULL;
}
