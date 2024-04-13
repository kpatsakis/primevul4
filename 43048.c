static u64 kvm_dr6_fixed(struct kvm_vcpu *vcpu)
{
	u64 fixed = DR6_FIXED_1;

	if (!guest_cpuid_has_rtm(vcpu))
		fixed |= DR6_RTM;
	return fixed;
}
