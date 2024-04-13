static inline void put_futex_key(union futex_key *key)
{
	drop_futex_key_refs(key);
}
