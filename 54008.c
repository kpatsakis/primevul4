static void vcpu_scan_ioapic(struct kvm_vcpu *vcpu)
{
	if (!kvm_apic_hw_enabled(vcpu->arch.apic))
		return;

	memset(vcpu->arch.eoi_exit_bitmap, 0, 256 / 8);

	if (irqchip_split(vcpu->kvm))
		kvm_scan_ioapic_routes(vcpu, vcpu->arch.eoi_exit_bitmap);
	else {
		kvm_x86_ops->sync_pir_to_irr(vcpu);
		kvm_ioapic_scan_entry(vcpu, vcpu->arch.eoi_exit_bitmap);
	}
	kvm_x86_ops->load_eoi_exitmap(vcpu);
}
