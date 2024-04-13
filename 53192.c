rsrc_cmp_key(const struct archive_rb_node *n, const void *key)
{
	const struct zip_entry *e = (const struct zip_entry *)n;
	return (strcmp((const char *)key, e->rsrcname.s));
}
