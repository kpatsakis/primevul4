int kvm_arch_vcpu_setup(struct kvm_vcpu *vcpu)
{
	int r;

	kvm_vcpu_mtrr_init(vcpu);
	r = vcpu_load(vcpu);
	if (r)
		return r;
	kvm_vcpu_reset(vcpu, false);
	kvm_mmu_setup(vcpu);
	vcpu_put(vcpu);
	return r;
}
