static struct ip_tunnel *ipgre_tunnel_find(struct net *net,
					   struct ip_tunnel_parm *parms,
					   int type)
{
	__be32 remote = parms->iph.daddr;
	__be32 local = parms->iph.saddr;
	__be32 key = parms->i_key;
	int link = parms->link;
	struct ip_tunnel *t;
	struct ip_tunnel __rcu **tp;
	struct ipgre_net *ign = net_generic(net, ipgre_net_id);

	for (tp = __ipgre_bucket(ign, parms);
	     (t = rtnl_dereference(*tp)) != NULL;
	     tp = &t->next)
		if (local == t->parms.iph.saddr &&
		    remote == t->parms.iph.daddr &&
		    key == t->parms.i_key &&
		    link == t->parms.link &&
		    type == t->dev->type)
			break;

	return t;
}
