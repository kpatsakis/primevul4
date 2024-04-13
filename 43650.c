static unsigned long *__gfn_to_rmap(gfn_t gfn, int level,
				    struct kvm_memory_slot *slot)
{
	unsigned long idx;

	idx = gfn_to_index(gfn, slot->base_gfn, level);
	return &slot->arch.rmap[level - PT_PAGE_TABLE_LEVEL][idx];
}
