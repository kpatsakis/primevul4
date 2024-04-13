static inline unsigned int fold_hash(unsigned long hash)
{
	return hash_64(hash, 32);
}
