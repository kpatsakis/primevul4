static inline u32 ping_hashfn(const struct net *net, u32 num, u32 mask)
{
	u32 res = (num + net_hash_mix(net)) & mask;

	pr_debug("hash(%u) = %u\n", num, res);
	return res;
}
