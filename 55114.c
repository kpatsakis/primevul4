void kvm_arch_vcpu_uninit(struct kvm_vcpu *vcpu)
{
	kvmppc_mmu_destroy(vcpu);
	kvmppc_subarch_vcpu_uninit(vcpu);
}
