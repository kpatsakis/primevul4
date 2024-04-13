int nfs_generic_flush(struct nfs_pageio_descriptor *desc,
		      struct nfs_pgio_header *hdr)
{
	if (desc->pg_bsize < PAGE_CACHE_SIZE)
		return nfs_flush_multi(desc, hdr);
	return nfs_flush_one(desc, hdr);
}
