static bool nested_get_vmcs12_pages(struct kvm_vcpu *vcpu,
					struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int maxphyaddr = cpuid_maxphyaddr(vcpu);

	if (nested_cpu_has2(vmcs12, SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)) {
		if (!PAGE_ALIGNED(vmcs12->apic_access_addr) ||
		    vmcs12->apic_access_addr >> maxphyaddr)
			return false;

		/*
		 * Translate L1 physical address to host physical
		 * address for vmcs02. Keep the page pinned, so this
		 * physical address remains valid. We keep a reference
		 * to it so we can release it later.
		 */
		if (vmx->nested.apic_access_page) /* shouldn't happen */
			nested_release_page(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page =
			nested_get_page(vcpu, vmcs12->apic_access_addr);
	}

	if (nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW)) {
		if (!PAGE_ALIGNED(vmcs12->virtual_apic_page_addr) ||
		    vmcs12->virtual_apic_page_addr >> maxphyaddr)
			return false;

		if (vmx->nested.virtual_apic_page) /* shouldn't happen */
			nested_release_page(vmx->nested.virtual_apic_page);
		vmx->nested.virtual_apic_page =
			nested_get_page(vcpu, vmcs12->virtual_apic_page_addr);

		/*
		 * Failing the vm entry is _not_ what the processor does
		 * but it's basically the only possibility we have.
		 * We could still enter the guest if CR8 load exits are
		 * enabled, CR8 store exits are enabled, and virtualize APIC
		 * access is disabled; in this case the processor would never
		 * use the TPR shadow and we could simply clear the bit from
		 * the execution control.  But such a configuration is useless,
		 * so let's keep the code simple.
		 */
		if (!vmx->nested.virtual_apic_page)
			return false;
	}

	if (nested_cpu_has_posted_intr(vmcs12)) {
		if (!IS_ALIGNED(vmcs12->posted_intr_desc_addr, 64) ||
		    vmcs12->posted_intr_desc_addr >> maxphyaddr)
			return false;

		if (vmx->nested.pi_desc_page) { /* shouldn't happen */
			kunmap(vmx->nested.pi_desc_page);
			nested_release_page(vmx->nested.pi_desc_page);
		}
		vmx->nested.pi_desc_page =
			nested_get_page(vcpu, vmcs12->posted_intr_desc_addr);
		if (!vmx->nested.pi_desc_page)
			return false;

		vmx->nested.pi_desc =
			(struct pi_desc *)kmap(vmx->nested.pi_desc_page);
		if (!vmx->nested.pi_desc) {
			nested_release_page_clean(vmx->nested.pi_desc_page);
			return false;
		}
		vmx->nested.pi_desc =
			(struct pi_desc *)((void *)vmx->nested.pi_desc +
			(unsigned long)(vmcs12->posted_intr_desc_addr &
			(PAGE_SIZE - 1)));
	}

	return true;
}
