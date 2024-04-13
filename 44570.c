static void nfs_set_pageerror(struct page *page)
{
	nfs_zap_mapping(page_file_mapping(page)->host, page_file_mapping(page));
}
