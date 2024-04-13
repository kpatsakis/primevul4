static void print_stats(struct xen_blkif *blkif)
{
	pr_info("xen-blkback (%s): oo %3llu  |  rd %4llu  |  wr %4llu  |  f %4llu"
		 "  |  ds %4llu | pg: %4u/%4d\n",
		 current->comm, blkif->st_oo_req,
		 blkif->st_rd_req, blkif->st_wr_req,
		 blkif->st_f_req, blkif->st_ds_req,
		 blkif->persistent_gnt_c,
		 xen_blkif_max_pgrants);
	blkif->st_print = jiffies + msecs_to_jiffies(10 * 1000);
	blkif->st_rd_req = 0;
	blkif->st_wr_req = 0;
	blkif->st_oo_req = 0;
	blkif->st_ds_req = 0;
}
