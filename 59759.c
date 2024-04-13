void nfs_fattr_init_names(struct nfs_fattr *fattr,
		struct nfs4_string *owner_name,
		struct nfs4_string *group_name)
{
	fattr->owner_name = owner_name;
	fattr->group_name = group_name;
}
