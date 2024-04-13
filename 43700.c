int kvm_age_hva(struct kvm *kvm, unsigned long hva)
{
	return kvm_handle_hva(kvm, hva, hva, kvm_age_rmapp);
}
