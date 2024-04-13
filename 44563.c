nfs_request_add_commit_list(struct nfs_page *req, struct list_head *dst,
			    struct nfs_commit_info *cinfo)
{
	set_bit(PG_CLEAN, &(req)->wb_flags);
	spin_lock(cinfo->lock);
	nfs_list_add_request(req, dst);
	cinfo->mds->ncommit++;
	spin_unlock(cinfo->lock);
	if (!cinfo->dreq) {
		inc_zone_page_state(req->wb_page, NR_UNSTABLE_NFS);
		inc_bdi_stat(page_file_mapping(req->wb_page)->backing_dev_info,
			     BDI_RECLAIMABLE);
		__mark_inode_dirty(req->wb_context->dentry->d_inode,
				   I_DIRTY_DATASYNC);
	}
}
