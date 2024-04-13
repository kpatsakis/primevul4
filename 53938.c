int kvm_get_dr(struct kvm_vcpu *vcpu, int dr, unsigned long *val)
{
	switch (dr) {
	case 0 ... 3:
		*val = vcpu->arch.db[dr];
		break;
	case 4:
		/* fall through */
	case 6:
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP)
			*val = vcpu->arch.dr6;
		else
			*val = kvm_x86_ops->get_dr6(vcpu);
		break;
	case 5:
		/* fall through */
	default: /* 7 */
		*val = vcpu->arch.dr7;
		break;
	}
	return 0;
}
