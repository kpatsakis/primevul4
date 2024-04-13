static void __tcp_alloc_md5sig_pool(void)
{
	struct crypto_ahash *hash;
	int cpu;

	hash = crypto_alloc_ahash("md5", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(hash))
		return;

	for_each_possible_cpu(cpu) {
		void *scratch = per_cpu(tcp_md5sig_pool, cpu).scratch;
		struct ahash_request *req;

		if (!scratch) {
			scratch = kmalloc_node(sizeof(union tcp_md5sum_block) +
					       sizeof(struct tcphdr),
					       GFP_KERNEL,
					       cpu_to_node(cpu));
			if (!scratch)
				return;
			per_cpu(tcp_md5sig_pool, cpu).scratch = scratch;
		}
		if (per_cpu(tcp_md5sig_pool, cpu).md5_req)
			continue;

		req = ahash_request_alloc(hash, GFP_KERNEL);
		if (!req)
			return;

		ahash_request_set_callback(req, 0, NULL, NULL);

		per_cpu(tcp_md5sig_pool, cpu).md5_req = req;
	}
	/* before setting tcp_md5sig_pool_populated, we must commit all writes
	 * to memory. See smp_rmb() in tcp_get_md5sig_pool()
	 */
	smp_wmb();
	tcp_md5sig_pool_populated = true;
}
