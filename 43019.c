static int _kvm_get_dr(struct kvm_vcpu *vcpu, int dr, unsigned long *val)
{
	switch (dr) {
	case 0 ... 3:
		*val = vcpu->arch.db[dr];
		break;
	case 4:
		if (kvm_read_cr4_bits(vcpu, X86_CR4_DE))
			return 1;
		/* fall through */
	case 6:
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP)
			*val = vcpu->arch.dr6;
		else
			*val = kvm_x86_ops->get_dr6(vcpu);
		break;
	case 5:
		if (kvm_read_cr4_bits(vcpu, X86_CR4_DE))
			return 1;
		/* fall through */
	default: /* 7 */
		*val = vcpu->arch.dr7;
		break;
	}

	return 0;
}
