static void vmx_recover_nmi_blocking(struct vcpu_vmx *vmx)
{
	u32 exit_intr_info;
	bool unblock_nmi;
	u8 vector;
	bool idtv_info_valid;

	idtv_info_valid = vmx->idt_vectoring_info & VECTORING_INFO_VALID_MASK;

	if (vmx->loaded_vmcs->nmi_known_unmasked)
		return;
	/*
	 * Can't use vmx->exit_intr_info since we're not sure what
	 * the exit reason is.
	 */
	exit_intr_info = vmcs_read32(VM_EXIT_INTR_INFO);
	unblock_nmi = (exit_intr_info & INTR_INFO_UNBLOCK_NMI) != 0;
	vector = exit_intr_info & INTR_INFO_VECTOR_MASK;
	/*
	 * SDM 3: 27.7.1.2 (September 2008)
	 * Re-set bit "block by NMI" before VM entry if vmexit caused by
	 * a guest IRET fault.
	 * SDM 3: 23.2.2 (September 2008)
	 * Bit 12 is undefined in any of the following cases:
	 *  If the VM exit sets the valid bit in the IDT-vectoring
	 *   information field.
	 *  If the VM exit is due to a double fault.
	 */
	if ((exit_intr_info & INTR_INFO_VALID_MASK) && unblock_nmi &&
	    vector != DF_VECTOR && !idtv_info_valid)
		vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
			      GUEST_INTR_STATE_NMI);
	else
		vmx->loaded_vmcs->nmi_known_unmasked =
			!(vmcs_read32(GUEST_INTERRUPTIBILITY_INFO)
			  & GUEST_INTR_STATE_NMI);
}
