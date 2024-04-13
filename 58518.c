static void set_max_drc(void)
{
	#define NFSD_DRC_SIZE_SHIFT	10
	nfsd_drc_max_mem = (nr_free_buffer_pages()
					>> NFSD_DRC_SIZE_SHIFT) * PAGE_SIZE;
	nfsd_drc_mem_used = 0;
	spin_lock_init(&nfsd_drc_lock);
	dprintk("%s nfsd_drc_max_mem %lu \n", __func__, nfsd_drc_max_mem);
}
