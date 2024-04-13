bool kvm_arch_intc_initialized(struct kvm *kvm)
{
#ifdef CONFIG_KVM_MPIC
	if (kvm->arch.mpic)
		return true;
#endif
#ifdef CONFIG_KVM_XICS
	if (kvm->arch.xics || kvm->arch.xive)
		return true;
#endif
	return false;
}
