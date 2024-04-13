static void ipip_tunnel_link(struct ipip_net *ipn, struct ip_tunnel *t)
{
	struct ip_tunnel __rcu **tp = ipip_bucket(ipn, t);

	rcu_assign_pointer(t->next, rtnl_dereference(*tp));
	rcu_assign_pointer(*tp, t);
}
