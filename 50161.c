static void req_prot_cleanup(struct request_sock_ops *rsk_prot)
{
	if (!rsk_prot)
		return;
	kfree(rsk_prot->slab_name);
	rsk_prot->slab_name = NULL;
	kmem_cache_destroy(rsk_prot->slab);
	rsk_prot->slab = NULL;
}
