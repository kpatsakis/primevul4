int emulator_get_dr(int dr, unsigned long *dest, struct kvm_vcpu *vcpu)
{
	return _kvm_get_dr(vcpu, dr, dest);
}
