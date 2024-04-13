static void **alloc_pg_vec(struct netlink_sock *nlk,
			   struct nl_mmap_req *req, unsigned int order)
{
	unsigned int block_nr = req->nm_block_nr;
	unsigned int i;
	void **pg_vec;

	pg_vec = kcalloc(block_nr, sizeof(void *), GFP_KERNEL);
	if (pg_vec == NULL)
		return NULL;

	for (i = 0; i < block_nr; i++) {
		pg_vec[i] = alloc_one_pg_vec_page(order);
		if (pg_vec[i] == NULL)
			goto err1;
	}

	return pg_vec;
err1:
	free_pg_vec(pg_vec, order, block_nr);
	return NULL;
}
