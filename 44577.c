static void nfs_write_completion(struct nfs_pgio_header *hdr)
{
	struct nfs_commit_info cinfo;
	unsigned long bytes = 0;

	if (test_bit(NFS_IOHDR_REDO, &hdr->flags))
		goto out;
	nfs_init_cinfo_from_inode(&cinfo, hdr->inode);
	while (!list_empty(&hdr->pages)) {
		struct nfs_page *req = nfs_list_entry(hdr->pages.next);

		bytes += req->wb_bytes;
		nfs_list_remove_request(req);
		if (test_bit(NFS_IOHDR_ERROR, &hdr->flags) &&
		    (hdr->good_bytes < bytes)) {
			nfs_set_pageerror(req->wb_page);
			nfs_context_set_write_error(req->wb_context, hdr->error);
			goto remove_req;
		}
		if (test_bit(NFS_IOHDR_NEED_RESCHED, &hdr->flags)) {
			nfs_mark_request_dirty(req);
			goto next;
		}
		if (test_bit(NFS_IOHDR_NEED_COMMIT, &hdr->flags)) {
			memcpy(&req->wb_verf, &hdr->verf->verifier, sizeof(req->wb_verf));
			nfs_mark_request_commit(req, hdr->lseg, &cinfo);
			goto next;
		}
remove_req:
		nfs_inode_remove_request(req);
next:
		nfs_unlock_request(req);
		nfs_end_page_writeback(req->wb_page);
		nfs_release_request(req);
	}
out:
	hdr->release(hdr);
}
