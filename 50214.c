void kvm_ioapic_update_eoi(struct kvm_vcpu *vcpu, int vector, int trigger_mode)
{
	struct kvm_ioapic *ioapic = vcpu->kvm->arch.vioapic;

	spin_lock(&ioapic->lock);
	__kvm_ioapic_update_eoi(vcpu, ioapic, vector, trigger_mode);
	spin_unlock(&ioapic->lock);
}
