static int hash_item(__be16 sid, unsigned char *addr)
{
	unsigned char hash = 0;
	unsigned int i;

	for (i = 0; i < ETH_ALEN; i++)
		hash ^= addr[i];
	for (i = 0; i < sizeof(sid_t) * 8; i += 8)
		hash ^= (__force __u32)sid >> i;
	for (i = 8; (i >>= 1) >= PPPOE_HASH_BITS;)
		hash ^= hash >> i;

	return hash & PPPOE_HASH_MASK;
}
