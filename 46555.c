char *nd_get_link(struct nameidata *nd)
{
	return nd->saved_names[nd->depth];
}
