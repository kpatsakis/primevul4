 static void paging_new_cr3(struct kvm_vcpu *vcpu)
 {
	pgprintk("%s: cr3 %lx\n", __func__, kvm_read_cr3(vcpu));
	mmu_free_roots(vcpu);
}
