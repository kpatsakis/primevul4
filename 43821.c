static void rmap_recycle(struct kvm_vcpu *vcpu, u64 *spte, gfn_t gfn)
{
	unsigned long *rmapp;
	struct kvm_mmu_page *sp;

	sp = page_header(__pa(spte));

	rmapp = gfn_to_rmap(vcpu->kvm, gfn, sp->role.level);

	kvm_unmap_rmapp(vcpu->kvm, rmapp, NULL, 0);
	kvm_flush_remote_tlbs(vcpu->kvm);
}
