static struct ip_tunnel * ipip_tunnel_lookup(struct net *net,
		__be32 remote, __be32 local)
{
	unsigned int h0 = HASH(remote);
	unsigned int h1 = HASH(local);
	struct ip_tunnel *t;
	struct ipip_net *ipn = net_generic(net, ipip_net_id);

	for_each_ip_tunnel_rcu(ipn->tunnels_r_l[h0 ^ h1])
		if (local == t->parms.iph.saddr &&
		    remote == t->parms.iph.daddr && (t->dev->flags&IFF_UP))
			return t;

	for_each_ip_tunnel_rcu(ipn->tunnels_r[h0])
		if (remote == t->parms.iph.daddr && (t->dev->flags&IFF_UP))
			return t;

	for_each_ip_tunnel_rcu(ipn->tunnels_l[h1])
		if (local == t->parms.iph.saddr && (t->dev->flags&IFF_UP))
			return t;

	t = rcu_dereference(ipn->tunnels_wc[0]);
	if (t && (t->dev->flags&IFF_UP))
		return t;
	return NULL;
}
