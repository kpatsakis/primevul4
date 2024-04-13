static enum nl_mmap_status netlink_get_status(const struct nl_mmap_hdr *hdr)
{
	smp_rmb();
	flush_dcache_page(pgvec_to_page(hdr));
	return hdr->nm_status;
}
