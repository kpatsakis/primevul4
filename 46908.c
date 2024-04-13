static int vmx_complete_nested_posted_interrupt(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int max_irr;
	void *vapic_page;
	u16 status;

	if (vmx->nested.pi_desc &&
	    vmx->nested.pi_pending) {
		vmx->nested.pi_pending = false;
		if (!pi_test_and_clear_on(vmx->nested.pi_desc))
			return 0;

		max_irr = find_last_bit(
			(unsigned long *)vmx->nested.pi_desc->pir, 256);

		if (max_irr == 256)
			return 0;

		vapic_page = kmap(vmx->nested.virtual_apic_page);
		if (!vapic_page) {
			WARN_ON(1);
			return -ENOMEM;
		}
		__kvm_apic_update_irr(vmx->nested.pi_desc->pir, vapic_page);
		kunmap(vmx->nested.virtual_apic_page);

		status = vmcs_read16(GUEST_INTR_STATUS);
		if ((u8)max_irr > ((u8)status & 0xff)) {
			status &= ~0xff;
			status |= (u8)max_irr;
			vmcs_write16(GUEST_INTR_STATUS, status);
		}
	}
	return 0;
}
