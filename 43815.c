static bool quickly_check_mmio_pf(struct kvm_vcpu *vcpu, u64 addr, bool direct)
{
	if (direct)
		return vcpu_match_mmio_gpa(vcpu, addr);

	return vcpu_match_mmio_gva(vcpu, addr);
}
