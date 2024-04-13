void kvm_arch_vcpu_free(struct kvm_vcpu *vcpu)
{
	if (vcpu->arch.time_page) {
		kvm_release_page_dirty(vcpu->arch.time_page);
		vcpu->arch.time_page = NULL;
	}

	free_cpumask_var(vcpu->arch.wbinvd_dirty_mask);
	fx_free(vcpu);
	kvm_x86_ops->vcpu_free(vcpu);
}
