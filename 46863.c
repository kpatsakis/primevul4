static void load_vmcs12_host_state(struct kvm_vcpu *vcpu,
				   struct vmcs12 *vmcs12)
{
	struct kvm_segment seg;

	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_EFER)
		vcpu->arch.efer = vmcs12->host_ia32_efer;
	else if (vmcs12->vm_exit_controls & VM_EXIT_HOST_ADDR_SPACE_SIZE)
		vcpu->arch.efer |= (EFER_LMA | EFER_LME);
	else
		vcpu->arch.efer &= ~(EFER_LMA | EFER_LME);
	vmx_set_efer(vcpu, vcpu->arch.efer);

	kvm_register_write(vcpu, VCPU_REGS_RSP, vmcs12->host_rsp);
	kvm_register_write(vcpu, VCPU_REGS_RIP, vmcs12->host_rip);
	vmx_set_rflags(vcpu, X86_EFLAGS_FIXED);
	/*
	 * Note that calling vmx_set_cr0 is important, even if cr0 hasn't
	 * actually changed, because it depends on the current state of
	 * fpu_active (which may have changed).
	 * Note that vmx_set_cr0 refers to efer set above.
	 */
	vmx_set_cr0(vcpu, vmcs12->host_cr0);
	/*
	 * If we did fpu_activate()/fpu_deactivate() during L2's run, we need
	 * to apply the same changes to L1's vmcs. We just set cr0 correctly,
	 * but we also need to update cr0_guest_host_mask and exception_bitmap.
	 */
	update_exception_bitmap(vcpu);
	vcpu->arch.cr0_guest_owned_bits = (vcpu->fpu_active ? X86_CR0_TS : 0);
	vmcs_writel(CR0_GUEST_HOST_MASK, ~vcpu->arch.cr0_guest_owned_bits);

	/*
	 * Note that CR4_GUEST_HOST_MASK is already set in the original vmcs01
	 * (KVM doesn't change it)- no reason to call set_cr4_guest_host_mask();
	 */
	vcpu->arch.cr4_guest_owned_bits = ~vmcs_readl(CR4_GUEST_HOST_MASK);
	kvm_set_cr4(vcpu, vmcs12->host_cr4);

	nested_ept_uninit_mmu_context(vcpu);

	kvm_set_cr3(vcpu, vmcs12->host_cr3);
	kvm_mmu_reset_context(vcpu);

	if (!enable_ept)
		vcpu->arch.walk_mmu->inject_page_fault = kvm_inject_page_fault;

	if (enable_vpid) {
		/*
		 * Trivially support vpid by letting L2s share their parent
		 * L1's vpid. TODO: move to a more elaborate solution, giving
		 * each L2 its own vpid and exposing the vpid feature to L1.
		 */
		vmx_flush_tlb(vcpu);
	}


	vmcs_write32(GUEST_SYSENTER_CS, vmcs12->host_ia32_sysenter_cs);
	vmcs_writel(GUEST_SYSENTER_ESP, vmcs12->host_ia32_sysenter_esp);
	vmcs_writel(GUEST_SYSENTER_EIP, vmcs12->host_ia32_sysenter_eip);
	vmcs_writel(GUEST_IDTR_BASE, vmcs12->host_idtr_base);
	vmcs_writel(GUEST_GDTR_BASE, vmcs12->host_gdtr_base);

	/* If not VM_EXIT_CLEAR_BNDCFGS, the L2 value propagates to L1.  */
	if (vmcs12->vm_exit_controls & VM_EXIT_CLEAR_BNDCFGS)
		vmcs_write64(GUEST_BNDCFGS, 0);

	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_PAT) {
		vmcs_write64(GUEST_IA32_PAT, vmcs12->host_ia32_pat);
		vcpu->arch.pat = vmcs12->host_ia32_pat;
	}
	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL)
		vmcs_write64(GUEST_IA32_PERF_GLOBAL_CTRL,
			vmcs12->host_ia32_perf_global_ctrl);

	/* Set L1 segment info according to Intel SDM
	    27.5.2 Loading Host Segment and Descriptor-Table Registers */
	seg = (struct kvm_segment) {
		.base = 0,
		.limit = 0xFFFFFFFF,
		.selector = vmcs12->host_cs_selector,
		.type = 11,
		.present = 1,
		.s = 1,
		.g = 1
	};
	if (vmcs12->vm_exit_controls & VM_EXIT_HOST_ADDR_SPACE_SIZE)
		seg.l = 1;
	else
		seg.db = 1;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_CS);
	seg = (struct kvm_segment) {
		.base = 0,
		.limit = 0xFFFFFFFF,
		.type = 3,
		.present = 1,
		.s = 1,
		.db = 1,
		.g = 1
	};
	seg.selector = vmcs12->host_ds_selector;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_DS);
	seg.selector = vmcs12->host_es_selector;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_ES);
	seg.selector = vmcs12->host_ss_selector;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_SS);
	seg.selector = vmcs12->host_fs_selector;
	seg.base = vmcs12->host_fs_base;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_FS);
	seg.selector = vmcs12->host_gs_selector;
	seg.base = vmcs12->host_gs_base;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_GS);
	seg = (struct kvm_segment) {
		.base = vmcs12->host_tr_base,
		.limit = 0x67,
		.selector = vmcs12->host_tr_selector,
		.type = 11,
		.present = 1
	};
	vmx_set_segment(vcpu, &seg, VCPU_SREG_TR);

	kvm_set_dr(vcpu, 7, 0x400);
	vmcs_write64(GUEST_IA32_DEBUGCTL, 0);

	if (cpu_has_vmx_msr_bitmap())
		vmx_set_msr_bitmap(vcpu);

	if (nested_vmx_load_msr(vcpu, vmcs12->vm_exit_msr_load_addr,
				vmcs12->vm_exit_msr_load_count))
		nested_vmx_abort(vcpu, VMX_ABORT_LOAD_HOST_MSR_FAIL);
}
