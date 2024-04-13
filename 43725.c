static void kvm_mmu_mark_parents_unsync(struct kvm_mmu_page *sp)
{
	pte_list_walk(&sp->parent_ptes, mark_unsync);
}
