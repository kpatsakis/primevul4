static void vmx_set_nmi_mask(struct kvm_vcpu *vcpu, bool masked)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	vmx->loaded_vmcs->nmi_known_unmasked = !masked;
	if (masked)
		vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
			      GUEST_INTR_STATE_NMI);
	else
		vmcs_clear_bits(GUEST_INTERRUPTIBILITY_INFO,
				GUEST_INTR_STATE_NMI);
}
