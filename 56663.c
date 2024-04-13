static void vmx_set_apic_access_page_addr(struct kvm_vcpu *vcpu, hpa_t hpa)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/*
	 * Currently we do not handle the nested case where L2 has an
	 * APIC access page of its own; that page is still pinned.
	 * Hence, we skip the case where the VCPU is in guest mode _and_
	 * L1 prepared an APIC access page for L2.
	 *
	 * For the case where L1 and L2 share the same APIC access page
	 * (flexpriority=Y but SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES clear
	 * in the vmcs12), this function will only update either the vmcs01
	 * or the vmcs02.  If the former, the vmcs02 will be updated by
	 * prepare_vmcs02.  If the latter, the vmcs01 will be updated in
	 * the next L2->L1 exit.
	 */
	if (!is_guest_mode(vcpu) ||
	    !nested_cpu_has2(get_vmcs12(&vmx->vcpu),
			     SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)) {
		vmcs_write64(APIC_ACCESS_ADDR, hpa);
		vmx_flush_tlb_ept_only(vcpu);
	}
}
