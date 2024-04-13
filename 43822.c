static void rmap_remove(struct kvm *kvm, u64 *spte)
{
	struct kvm_mmu_page *sp;
	gfn_t gfn;
	unsigned long *rmapp;

	sp = page_header(__pa(spte));
	gfn = kvm_mmu_page_get_gfn(sp, spte - sp->spt);
	rmapp = gfn_to_rmap(kvm, gfn, sp->role.level);
	pte_list_remove(spte, rmapp);
}
