static inline ext4_lblk_t dx_get_block(struct dx_entry *entry)
{
	return le32_to_cpu(entry->block) & 0x00ffffff;
}
