static inline struct hlist_head *nl_portid_hashfn(struct nl_portid_hash *hash, u32 portid)
{
	return &hash->table[jhash_1word(portid, hash->rnd) & hash->mask];
}
