xfs_default_hashname(
	struct xfs_name	*name)
{
	return xfs_da_hashname(name->name, name->len);
}
