static void nested_vmx_vmexit(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int cpu;
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	/* trying to cancel vmlaunch/vmresume is a bug */
	WARN_ON_ONCE(vmx->nested.nested_run_pending);

	leave_guest_mode(vcpu);
	prepare_vmcs12(vcpu, vmcs12);

	cpu = get_cpu();
	vmx->loaded_vmcs = &vmx->vmcs01;
	vmx_vcpu_put(vcpu);
	vmx_vcpu_load(vcpu, cpu);
	vcpu->cpu = cpu;
	put_cpu();

	vmx_segment_cache_clear(vmx);

	/* if no vmcs02 cache requested, remove the one we used */
	if (VMCS02_POOL_SIZE == 0)
		nested_free_vmcs02(vmx, vmx->nested.current_vmptr);

	load_vmcs12_host_state(vcpu, vmcs12);

	/* Update TSC_OFFSET if TSC was changed while L2 ran */
	vmcs_write64(TSC_OFFSET, vmx->nested.vmcs01_tsc_offset);

	/* This is needed for same reason as it was needed in prepare_vmcs02 */
	vmx->host_rsp = 0;

	/* Unpin physical memory we referred to in vmcs02 */
	if (vmx->nested.apic_access_page) {
		nested_release_page(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page = 0;
	}

	/*
	 * Exiting from L2 to L1, we're now back to L1 which thinks it just
	 * finished a VMLAUNCH or VMRESUME instruction, so we need to set the
	 * success or failure flag accordingly.
	 */
	if (unlikely(vmx->fail)) {
		vmx->fail = 0;
		nested_vmx_failValid(vcpu, vmcs_read32(VM_INSTRUCTION_ERROR));
	} else
		nested_vmx_succeed(vcpu);
	if (enable_shadow_vmcs)
		vmx->nested.sync_shadow_vmcs = true;
}
