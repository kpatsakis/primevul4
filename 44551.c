nfs_mark_request_commit(struct nfs_page *req, struct pnfs_layout_segment *lseg,
			struct nfs_commit_info *cinfo)
{
	if (pnfs_mark_request_commit(req, lseg, cinfo))
		return;
	nfs_request_add_commit_list(req, &cinfo->mds->list, cinfo);
}
