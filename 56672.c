static int vmx_sync_pir_to_irr(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int max_irr;

	WARN_ON(!vcpu->arch.apicv_active);
	if (pi_test_on(&vmx->pi_desc)) {
		pi_clear_on(&vmx->pi_desc);
		/*
		 * IOMMU can write to PIR.ON, so the barrier matters even on UP.
		 * But on x86 this is just a compiler barrier anyway.
		 */
		smp_mb__after_atomic();
		max_irr = kvm_apic_update_irr(vcpu, vmx->pi_desc.pir);
	} else {
		max_irr = kvm_lapic_find_highest_irr(vcpu);
	}
	vmx_hwapic_irr_update(vcpu, max_irr);
	return max_irr;
}
