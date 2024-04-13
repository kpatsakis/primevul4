cmp_node(const struct archive_rb_node *n1, const struct archive_rb_node *n2)
{
	const struct zip_entry *e1 = (const struct zip_entry *)n1;
	const struct zip_entry *e2 = (const struct zip_entry *)n2;

	if (e1->local_header_offset > e2->local_header_offset)
		return -1;
	if (e1->local_header_offset < e2->local_header_offset)
		return 1;
	return 0;
}
