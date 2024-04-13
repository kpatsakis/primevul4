int ext4_find_delalloc_range(struct inode *inode,
			     ext4_lblk_t lblk_start,
			     ext4_lblk_t lblk_end)
{
	struct extent_status es;

	ext4_es_find_delayed_extent_range(inode, lblk_start, lblk_end, &es);
	if (es.es_len == 0)
		return 0; /* there is no delay extent in this tree */
	else if (es.es_lblk <= lblk_start &&
		 lblk_start < es.es_lblk + es.es_len)
		return 1;
	else if (lblk_start <= es.es_lblk && es.es_lblk <= lblk_end)
		return 1;
	else
		return 0;
}
