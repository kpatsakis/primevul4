u64 vmx_read_l1_tsc(struct kvm_vcpu *vcpu, u64 host_tsc)
{
	u64 tsc_offset;

	tsc_offset = is_guest_mode(vcpu) ?
		to_vmx(vcpu)->nested.vmcs01_tsc_offset :
		vmcs_read64(TSC_OFFSET);
	return host_tsc + tsc_offset;
}
