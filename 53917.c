void kvm_arch_vcpu_free(struct kvm_vcpu *vcpu)
{
	kvmclock_reset(vcpu);

	free_cpumask_var(vcpu->arch.wbinvd_dirty_mask);
	kvm_x86_ops->vcpu_free(vcpu);
}
