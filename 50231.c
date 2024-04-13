static int nf_ct_net_init(struct net *net)
{
	int res;

	net->nf_frag.frags.high_thresh = IPV6_FRAG_HIGH_THRESH;
	net->nf_frag.frags.low_thresh = IPV6_FRAG_LOW_THRESH;
	net->nf_frag.frags.timeout = IPV6_FRAG_TIMEOUT;
	res = inet_frags_init_net(&net->nf_frag.frags);
	if (res)
		return res;
	res = nf_ct_frag6_sysctl_register(net);
	if (res)
		inet_frags_uninit_net(&net->nf_frag.frags);
	return res;
}
