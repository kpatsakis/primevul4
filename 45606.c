static void netlink_set_status(struct nl_mmap_hdr *hdr,
			       enum nl_mmap_status status)
{
	hdr->nm_status = status;
	flush_dcache_page(pgvec_to_page(hdr));
	smp_wmb();
}
