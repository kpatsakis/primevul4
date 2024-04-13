static void nfs_fattr_free_group_name(struct nfs_fattr *fattr)
{
	fattr->valid &= ~NFS_ATTR_FATTR_GROUP_NAME;
	kfree(fattr->group_name->data);
}
