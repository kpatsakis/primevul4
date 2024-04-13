static void nested_mark_vmcs12_pages_dirty(struct kvm_vcpu *vcpu)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	gfn_t gfn;

	/*
	 * Don't need to mark the APIC access page dirty; it is never
	 * written to by the CPU during APIC virtualization.
	 */

	if (nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW)) {
		gfn = vmcs12->virtual_apic_page_addr >> PAGE_SHIFT;
		kvm_vcpu_mark_page_dirty(vcpu, gfn);
	}

	if (nested_cpu_has_posted_intr(vmcs12)) {
		gfn = vmcs12->posted_intr_desc_addr >> PAGE_SHIFT;
		kvm_vcpu_mark_page_dirty(vcpu, gfn);
	}
}
