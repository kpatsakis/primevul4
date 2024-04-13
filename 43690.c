static int init_kvm_softmmu(struct kvm_vcpu *vcpu)
{
	int r = kvm_init_shadow_mmu(vcpu, vcpu->arch.walk_mmu);

	vcpu->arch.walk_mmu->set_cr3           = kvm_x86_ops->set_cr3;
	vcpu->arch.walk_mmu->get_cr3           = get_cr3;
	vcpu->arch.walk_mmu->get_pdptr         = kvm_pdptr_read;
	vcpu->arch.walk_mmu->inject_page_fault = kvm_inject_page_fault;

	return r;
}
