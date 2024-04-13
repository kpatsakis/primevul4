static u32 udp4_portaddr_hash(const struct net *net, __be32 saddr,
			      unsigned int port)
{
	return jhash_1word((__force u32)saddr, net_hash_mix(net)) ^ port;
}
