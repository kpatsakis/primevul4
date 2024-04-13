static void netlink_frame_flush_dcache(const struct nl_mmap_hdr *hdr)
{
#if ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE == 1
	struct page *p_start, *p_end;

	/* First page is flushed through netlink_{get,set}_status */
	p_start = pgvec_to_page(hdr + PAGE_SIZE);
	p_end   = pgvec_to_page((void *)hdr + NL_MMAP_HDRLEN + hdr->nm_len - 1);
	while (p_start <= p_end) {
		flush_dcache_page(p_start);
		p_start++;
	}
#endif
}
