static u32 udp_ehashfn(const struct net *net, const __be32 laddr,
		       const __u16 lport, const __be32 faddr,
		       const __be16 fport)
{
	static u32 udp_ehash_secret __read_mostly;

	net_get_random_once(&udp_ehash_secret, sizeof(udp_ehash_secret));

	return __inet_ehashfn(laddr, lport, faddr, fport,
			      udp_ehash_secret + net_hash_mix(net));
}
