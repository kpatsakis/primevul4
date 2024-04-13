void put_futex_key(int fshared, union futex_key *key)
{
	drop_futex_key_refs(key);
}
