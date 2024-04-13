static void vmx_load_eoi_exitmap(struct kvm_vcpu *vcpu)
{
	u64 *eoi_exit_bitmap = vcpu->arch.eoi_exit_bitmap;
	if (!vmx_cpu_uses_apicv(vcpu))
		return;

	vmcs_write64(EOI_EXIT_BITMAP0, eoi_exit_bitmap[0]);
	vmcs_write64(EOI_EXIT_BITMAP1, eoi_exit_bitmap[1]);
	vmcs_write64(EOI_EXIT_BITMAP2, eoi_exit_bitmap[2]);
	vmcs_write64(EOI_EXIT_BITMAP3, eoi_exit_bitmap[3]);
}
