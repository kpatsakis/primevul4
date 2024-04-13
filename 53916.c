void kvm_arch_vcpu_destroy(struct kvm_vcpu *vcpu)
{
	int r;
	vcpu->arch.apf.msr_val = 0;

	r = vcpu_load(vcpu);
	BUG_ON(r);
	kvm_mmu_unload(vcpu);
	vcpu_put(vcpu);

	kvm_x86_ops->vcpu_free(vcpu);
}
