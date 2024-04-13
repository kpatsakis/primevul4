static int dccp_init_net(struct net *net, u_int16_t proto)
{
	struct dccp_net *dn = dccp_pernet(net);
	struct nf_proto_net *pn = &dn->pn;

	if (!pn->users) {
		/* default values */
		dn->dccp_loose = 1;
		dn->dccp_timeout[CT_DCCP_REQUEST]	= 2 * DCCP_MSL;
		dn->dccp_timeout[CT_DCCP_RESPOND]	= 4 * DCCP_MSL;
		dn->dccp_timeout[CT_DCCP_PARTOPEN]	= 4 * DCCP_MSL;
		dn->dccp_timeout[CT_DCCP_OPEN]		= 12 * 3600 * HZ;
		dn->dccp_timeout[CT_DCCP_CLOSEREQ]	= 64 * HZ;
		dn->dccp_timeout[CT_DCCP_CLOSING]	= 64 * HZ;
		dn->dccp_timeout[CT_DCCP_TIMEWAIT]	= 2 * DCCP_MSL;
	}

	return dccp_kmemdup_sysctl_table(net, pn, dn);
}
