static int paging32E_init_context(struct kvm_vcpu *vcpu,
				  struct kvm_mmu *context)
{
	return paging64_init_context_common(vcpu, context, PT32E_ROOT_LEVEL);
}
