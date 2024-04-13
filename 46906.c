static void vmx_adjust_tsc_offset_guest(struct kvm_vcpu *vcpu, s64 adjustment)
{
	u64 offset = vmcs_read64(TSC_OFFSET);

	vmcs_write64(TSC_OFFSET, offset + adjustment);
	if (is_guest_mode(vcpu)) {
		/* Even when running L2, the adjustment needs to apply to L1 */
		to_vmx(vcpu)->nested.vmcs01_tsc_offset += adjustment;
	} else
		trace_kvm_write_tsc_offset(vcpu->vcpu_id, offset,
					   offset + adjustment);
}
