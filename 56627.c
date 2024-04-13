static void nested_vmx_vmexit(struct kvm_vcpu *vcpu, u32 exit_reason,
			      u32 exit_intr_info,
			      unsigned long exit_qualification)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	u32 vm_inst_error = 0;

	/* trying to cancel vmlaunch/vmresume is a bug */
	WARN_ON_ONCE(vmx->nested.nested_run_pending);

	leave_guest_mode(vcpu);
	prepare_vmcs12(vcpu, vmcs12, exit_reason, exit_intr_info,
		       exit_qualification);

	if (nested_vmx_store_msr(vcpu, vmcs12->vm_exit_msr_store_addr,
				 vmcs12->vm_exit_msr_store_count))
		nested_vmx_abort(vcpu, VMX_ABORT_SAVE_GUEST_MSR_FAIL);

	if (unlikely(vmx->fail))
		vm_inst_error = vmcs_read32(VM_INSTRUCTION_ERROR);

	vmx_switch_vmcs(vcpu, &vmx->vmcs01);

	/*
	 * TODO: SDM says that with acknowledge interrupt on exit, bit 31 of
	 * the VM-exit interrupt information (valid interrupt) is always set to
	 * 1 on EXIT_REASON_EXTERNAL_INTERRUPT, so we shouldn't need
	 * kvm_cpu_has_interrupt().  See the commit message for details.
	 */
	if (nested_exit_intr_ack_set(vcpu) &&
	    exit_reason == EXIT_REASON_EXTERNAL_INTERRUPT &&
	    kvm_cpu_has_interrupt(vcpu)) {
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

	vm_entry_controls_reset_shadow(vmx);
	vm_exit_controls_reset_shadow(vmx);
	vmx_segment_cache_clear(vmx);

	/* if no vmcs02 cache requested, remove the one we used */
	if (VMCS02_POOL_SIZE == 0)
		nested_free_vmcs02(vmx, vmx->nested.current_vmptr);

	load_vmcs12_host_state(vcpu, vmcs12);

	/* Update any VMCS fields that might have changed while L2 ran */
	vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, vmx->msr_autoload.nr);
	vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, vmx->msr_autoload.nr);
	vmcs_write64(TSC_OFFSET, vcpu->arch.tsc_offset);
	if (vmx->hv_deadline_tsc == -1)
		vmcs_clear_bits(PIN_BASED_VM_EXEC_CONTROL,
				PIN_BASED_VMX_PREEMPTION_TIMER);
	else
		vmcs_set_bits(PIN_BASED_VM_EXEC_CONTROL,
			      PIN_BASED_VMX_PREEMPTION_TIMER);
	if (kvm_has_tsc_control)
		decache_tsc_multiplier(vmx);

	if (vmx->nested.change_vmcs01_virtual_x2apic_mode) {
		vmx->nested.change_vmcs01_virtual_x2apic_mode = false;
		vmx_set_virtual_x2apic_mode(vcpu,
				vcpu->arch.apic_base & X2APIC_ENABLE);
	} else if (!nested_cpu_has_ept(vmcs12) &&
		   nested_cpu_has2(vmcs12,
				   SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)) {
		vmx_flush_tlb_ept_only(vcpu);
	}

	/* This is needed for same reason as it was needed in prepare_vmcs02 */
	vmx->host_rsp = 0;

	/* Unpin physical memory we referred to in vmcs02 */
	if (vmx->nested.apic_access_page) {
		kvm_release_page_dirty(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page = NULL;
	}
	if (vmx->nested.virtual_apic_page) {
		kvm_release_page_dirty(vmx->nested.virtual_apic_page);
		vmx->nested.virtual_apic_page = NULL;
	}
	if (vmx->nested.pi_desc_page) {
		kunmap(vmx->nested.pi_desc_page);
		kvm_release_page_dirty(vmx->nested.pi_desc_page);
		vmx->nested.pi_desc_page = NULL;
		vmx->nested.pi_desc = NULL;
	}

	/*
	 * We are now running in L2, mmu_notifier will force to reload the
	 * page's hpa for L2 vmcs. Need to reload it for L1 before entering L1.
	 */
	kvm_make_request(KVM_REQ_APIC_PAGE_RELOAD, vcpu);

	/*
	 * Exiting from L2 to L1, we're now back to L1 which thinks it just
	 * finished a VMLAUNCH or VMRESUME instruction, so we need to set the
	 * success or failure flag accordingly.
	 */
	if (unlikely(vmx->fail)) {
		vmx->fail = 0;
		nested_vmx_failValid(vcpu, vm_inst_error);
	} else
		nested_vmx_succeed(vcpu);
	if (enable_shadow_vmcs)
		vmx->nested.sync_shadow_vmcs = true;

	/* in case we halted in L2 */
	vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;
}
