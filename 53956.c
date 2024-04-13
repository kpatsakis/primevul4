int kvm_set_dr(struct kvm_vcpu *vcpu, int dr, unsigned long val)
{
	if (__kvm_set_dr(vcpu, dr, val)) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}
	return 0;
}
