nfs_clear_page_commit(struct page *page)
{
	dec_zone_page_state(page, NR_UNSTABLE_NFS);
	dec_bdi_stat(page_file_mapping(page)->backing_dev_info, BDI_RECLAIMABLE);
}
