void nfs_retry_commit(struct list_head *page_list,
		      struct pnfs_layout_segment *lseg,
		      struct nfs_commit_info *cinfo)
{
	struct nfs_page *req;

	while (!list_empty(page_list)) {
		req = nfs_list_entry(page_list->next);
		nfs_list_remove_request(req);
		nfs_mark_request_commit(req, lseg, cinfo);
		if (!cinfo->dreq) {
			dec_zone_page_state(req->wb_page, NR_UNSTABLE_NFS);
			dec_bdi_stat(page_file_mapping(req->wb_page)->backing_dev_info,
				     BDI_RECLAIMABLE);
		}
		nfs_unlock_and_release_request(req);
	}
}
