int kvm_arch_vcpu_setup(struct kvm_vcpu *vcpu)
{
	int r;

	vcpu->arch.mtrr_state.have_fixed = 1;
	vcpu_load(vcpu);
	r = kvm_arch_vcpu_reset(vcpu);
	if (r == 0)
		r = kvm_mmu_setup(vcpu);
	vcpu_put(vcpu);
	if (r < 0)
		goto free_vcpu;

	return 0;
free_vcpu:
	kvm_x86_ops->vcpu_free(vcpu);
	return r;
}
