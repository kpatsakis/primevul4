static unsigned long emulator_get_cr(int cr, struct kvm_vcpu *vcpu)
{
	unsigned long value;

	switch (cr) {
	case 0:
		value = kvm_read_cr0(vcpu);
		break;
	case 2:
		value = vcpu->arch.cr2;
		break;
	case 3:
		value = vcpu->arch.cr3;
		break;
	case 4:
		value = kvm_read_cr4(vcpu);
		break;
	case 8:
		value = kvm_get_cr8(vcpu);
		break;
	default:
		vcpu_printf(vcpu, "%s: unexpected cr %u\n", __func__, cr);
		return 0;
	}

	return value;
}
