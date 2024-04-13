static inline void dx_set_hash(struct dx_entry *entry, unsigned value)
{
	entry->hash = cpu_to_le32(value);
}
