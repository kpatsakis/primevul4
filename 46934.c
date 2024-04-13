static int vmx_set_cr4(struct kvm_vcpu *vcpu, unsigned long cr4)
{
	/*
	 * Pass through host's Machine Check Enable value to hw_cr4, which
	 * is in force while we are in guest mode.  Do not let guests control
	 * this bit, even if host CR4.MCE == 0.
	 */
	unsigned long hw_cr4 =
		(cr4_read_shadow() & X86_CR4_MCE) |
		(cr4 & ~X86_CR4_MCE) |
		(to_vmx(vcpu)->rmode.vm86_active ?
		 KVM_RMODE_VM_CR4_ALWAYS_ON : KVM_PMODE_VM_CR4_ALWAYS_ON);

	if (cr4 & X86_CR4_VMXE) {
		/*
		 * To use VMXON (and later other VMX instructions), a guest
		 * must first be able to turn on cr4.VMXE (see handle_vmon()).
		 * So basically the check on whether to allow nested VMX
		 * is here.
		 */
		if (!nested_vmx_allowed(vcpu))
			return 1;
	}
	if (to_vmx(vcpu)->nested.vmxon &&
	    ((cr4 & VMXON_CR4_ALWAYSON) != VMXON_CR4_ALWAYSON))
		return 1;

	vcpu->arch.cr4 = cr4;
	if (enable_ept) {
		if (!is_paging(vcpu)) {
			hw_cr4 &= ~X86_CR4_PAE;
			hw_cr4 |= X86_CR4_PSE;
		} else if (!(cr4 & X86_CR4_PAE)) {
			hw_cr4 &= ~X86_CR4_PAE;
		}
	}

	if (!enable_unrestricted_guest && !is_paging(vcpu))
		/*
		 * SMEP/SMAP is disabled if CPU is in non-paging mode in
		 * hardware.  However KVM always uses paging mode without
		 * unrestricted guest.
		 * To emulate this behavior, SMEP/SMAP needs to be manually
		 * disabled when guest switches to non-paging mode.
		 */
		hw_cr4 &= ~(X86_CR4_SMEP | X86_CR4_SMAP);

	vmcs_writel(CR4_READ_SHADOW, cr4);
	vmcs_writel(GUEST_CR4, hw_cr4);
	return 0;
}
