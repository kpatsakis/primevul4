void nfs_destroy_writepagecache(void)
{
	mempool_destroy(nfs_commit_mempool);
	kmem_cache_destroy(nfs_cdata_cachep);
	mempool_destroy(nfs_wdata_mempool);
	kmem_cache_destroy(nfs_wdata_cachep);
}
