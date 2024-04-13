static int vmx_nmi_allowed(struct kvm_vcpu *vcpu)
{
	if (to_vmx(vcpu)->nested.nested_run_pending)
		return 0;

	return	!(vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
		  (GUEST_INTR_STATE_MOV_SS | GUEST_INTR_STATE_STI
		   | GUEST_INTR_STATE_NMI));
}
