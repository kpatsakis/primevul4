static void vmx_set_rflags(struct kvm_vcpu *vcpu, unsigned long rflags)
{
	unsigned long old_rflags = vmx_get_rflags(vcpu);

	__set_bit(VCPU_EXREG_RFLAGS, (ulong *)&vcpu->arch.regs_avail);
	to_vmx(vcpu)->rflags = rflags;
	if (to_vmx(vcpu)->rmode.vm86_active) {
		to_vmx(vcpu)->rmode.save_rflags = rflags;
		rflags |= X86_EFLAGS_IOPL | X86_EFLAGS_VM;
	}
	vmcs_writel(GUEST_RFLAGS, rflags);

	if ((old_rflags ^ to_vmx(vcpu)->rflags) & X86_EFLAGS_VM)
		to_vmx(vcpu)->emulation_required = emulation_required(vcpu);
}
