static void ipgre_tunnel_link(struct ipgre_net *ign, struct ip_tunnel *t)
{
	struct ip_tunnel __rcu **tp = ipgre_bucket(ign, t);

	rcu_assign_pointer(t->next, rtnl_dereference(*tp));
	rcu_assign_pointer(*tp, t);
}
