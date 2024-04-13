rsrc_cmp_node(const struct archive_rb_node *n1,
    const struct archive_rb_node *n2)
{
	const struct zip_entry *e1 = (const struct zip_entry *)n1;
	const struct zip_entry *e2 = (const struct zip_entry *)n2;

	return (strcmp(e2->rsrcname.s, e1->rsrcname.s));
}
