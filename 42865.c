static inline struct ip_tunnel __rcu **ipip_bucket(struct ipip_net *ipn,
		struct ip_tunnel *t)
{
	return __ipip_bucket(ipn, &t->parms);
}
