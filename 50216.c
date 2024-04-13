int kvm_set_ioapic(struct kvm *kvm, struct kvm_ioapic_state *state)
{
	struct kvm_ioapic *ioapic = ioapic_irqchip(kvm);
	if (!ioapic)
		return -EINVAL;

	spin_lock(&ioapic->lock);
	memcpy(ioapic, state, sizeof(struct kvm_ioapic_state));
	ioapic->irr = 0;
	ioapic->irr_delivered = 0;
	kvm_vcpu_request_scan_ioapic(kvm);
	kvm_ioapic_inject_all(ioapic, state->irr);
	spin_unlock(&ioapic->lock);
	return 0;
}
