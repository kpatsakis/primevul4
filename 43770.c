static int mmu_memory_cache_free_objects(struct kvm_mmu_memory_cache *cache)
{
	return cache->nobjs;
}
