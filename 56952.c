static void print_stats(struct xen_blkif_ring *ring)
{
	pr_info("(%s): oo %3llu  |  rd %4llu  |  wr %4llu  |  f %4llu"
		 "  |  ds %4llu | pg: %4u/%4d\n",
		 current->comm, ring->st_oo_req,
		 ring->st_rd_req, ring->st_wr_req,
		 ring->st_f_req, ring->st_ds_req,
		 ring->persistent_gnt_c,
		 xen_blkif_max_pgrants);
	ring->st_print = jiffies + msecs_to_jiffies(10 * 1000);
	ring->st_rd_req = 0;
	ring->st_wr_req = 0;
	ring->st_oo_req = 0;
	ring->st_ds_req = 0;
}
