void kvm_vcpu_request_scan_ioapic(struct kvm *kvm)
{
	struct kvm_ioapic *ioapic = kvm->arch.vioapic;

	if (!ioapic)
		return;
	kvm_make_scan_ioapic_request(kvm);
}
