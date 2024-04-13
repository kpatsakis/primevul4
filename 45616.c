static inline int nl_portid_hash_dilute(struct nl_portid_hash *hash, int len)
{
	int avg = hash->entries >> hash->shift;

	if (unlikely(avg > 1) && nl_portid_hash_rehash(hash, 1))
		return 1;

	if (unlikely(len > avg) && time_after(jiffies, hash->rehash_time)) {
		nl_portid_hash_rehash(hash, 0);
		return 1;
	}

	return 0;
}
