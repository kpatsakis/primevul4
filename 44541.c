void nfs_init_cinfo(struct nfs_commit_info *cinfo,
		    struct inode *inode,
		    struct nfs_direct_req *dreq)
{
	if (dreq)
		nfs_init_cinfo_from_dreq(cinfo, dreq);
	else
		nfs_init_cinfo_from_inode(cinfo, inode);
}
