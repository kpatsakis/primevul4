static void nested_vmx_vmexit(struct kvm_vcpu *vcpu, u32 exit_reason,
			      u32 exit_intr_info,
			      unsigned long exit_qualification)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	/* trying to cancel vmlaunch/vmresume is a bug */
	WARN_ON_ONCE(vmx->nested.nested_run_pending);

	leave_guest_mode(vcpu);
	prepare_vmcs12(vcpu, vmcs12, exit_reason, exit_intr_info,
		       exit_qualification);

	if (nested_vmx_store_msr(vcpu, vmcs12->vm_exit_msr_store_addr,
				 vmcs12->vm_exit_msr_store_count))
		nested_vmx_abort(vcpu, VMX_ABORT_SAVE_GUEST_MSR_FAIL);

	vmx_load_vmcs01(vcpu);

	if ((exit_reason == EXIT_REASON_EXTERNAL_INTERRUPT)
	    && nested_exit_intr_ack_set(vcpu)) {
		int irq = kvm_cpu_get_interrupt(vcpu);
		WARN_ON(irq < 0);
		vmcs12->vm_exit_intr_info = irq |
			INTR_INFO_VALID_MASK | INTR_TYPE_EXT_INTR;
	}

	trace_kvm_nested_vmexit_inject(vmcs12->vm_exit_reason,
				       vmcs12->exit_qualification,
				       vmcs12->idt_vectoring_info_field,
				       vmcs12->vm_exit_intr_info,
				       vmcs12->vm_exit_intr_error_code,
				       KVM_ISA_VMX);

	vm_entry_controls_init(vmx, vmcs_read32(VM_ENTRY_CONTROLS));
	vm_exit_controls_init(vmx, vmcs_read32(VM_EXIT_CONTROLS));
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
		vmx->nested.apic_access_page = NULL;
	}
	if (vmx->nested.virtual_apic_page) {
		nested_release_page(vmx->nested.virtual_apic_page);
		vmx->nested.virtual_apic_page = NULL;
	}
	if (vmx->nested.pi_desc_page) {
		kunmap(vmx->nested.pi_desc_page);
		nested_release_page(vmx->nested.pi_desc_page);
		vmx->nested.pi_desc_page = NULL;
		vmx->nested.pi_desc = NULL;
	}

	/*
	 * We are now running in L2, mmu_notifier will force to reload the
	 * page's hpa for L2 vmcs. Need to reload it for L1 before entering L1.
	 */
	kvm_vcpu_reload_apic_access_page(vcpu);

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

	/* in case we halted in L2 */
	vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;
}
