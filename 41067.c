static inline unsigned dx_get_hash(struct dx_entry *entry)
{
	return le32_to_cpu(entry->hash);
}
