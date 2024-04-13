nfs_reqs_to_commit(struct nfs_commit_info *cinfo)
{
	return cinfo->mds->ncommit;
}
