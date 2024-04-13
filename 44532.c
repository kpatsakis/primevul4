static void nfs_end_page_writeback(struct page *page)
{
	struct inode *inode = page_file_mapping(page)->host;
	struct nfs_server *nfss = NFS_SERVER(inode);

	end_page_writeback(page);
	if (atomic_long_dec_return(&nfss->writeback) < NFS_CONGESTION_OFF_THRESH)
		clear_bdi_congested(&nfss->backing_dev_info, BLK_RW_ASYNC);
}
