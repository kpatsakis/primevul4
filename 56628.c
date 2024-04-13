static bool page_address_valid(struct kvm_vcpu *vcpu, gpa_t gpa)
{
	return PAGE_ALIGNED(gpa) && !(gpa >> cpuid_maxphyaddr(vcpu));
}
