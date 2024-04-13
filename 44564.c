nfs_request_remove_commit_list(struct nfs_page *req,
			       struct nfs_commit_info *cinfo)
{
	if (!test_and_clear_bit(PG_CLEAN, &(req)->wb_flags))
		return;
	nfs_list_remove_request(req);
	cinfo->mds->ncommit--;
}
