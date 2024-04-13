static int dccp_timeout_nlattr_to_obj(struct nlattr *tb[],
				      struct net *net, void *data)
{
	struct dccp_net *dn = dccp_pernet(net);
	unsigned int *timeouts = data;
	int i;

	/* set default DCCP timeouts. */
	for (i=0; i<CT_DCCP_MAX; i++)
		timeouts[i] = dn->dccp_timeout[i];

	/* there's a 1:1 mapping between attributes and protocol states. */
	for (i=CTA_TIMEOUT_DCCP_UNSPEC+1; i<CTA_TIMEOUT_DCCP_MAX+1; i++) {
		if (tb[i]) {
			timeouts[i] = ntohl(nla_get_be32(tb[i])) * HZ;
		}
	}
	return 0;
}
