static void vmx_apicv_post_state_restore(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	pi_clear_on(&vmx->pi_desc);
	memset(vmx->pi_desc.pir, 0, sizeof(vmx->pi_desc.pir));
}
