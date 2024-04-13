unsigned long kvm_get_linear_rip(struct kvm_vcpu *vcpu)
{
	if (is_64_bit_mode(vcpu))
		return kvm_rip_read(vcpu);
	return (u32)(get_segment_base(vcpu, VCPU_SREG_CS) +
		     kvm_rip_read(vcpu));
}
