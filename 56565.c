static void __vmx_load_host_state(struct vcpu_vmx *vmx)
{
	if (!vmx->host_state.loaded)
		return;

	++vmx->vcpu.stat.host_state_reload;
	vmx->host_state.loaded = 0;
#ifdef CONFIG_X86_64
	if (is_long_mode(&vmx->vcpu))
		rdmsrl(MSR_KERNEL_GS_BASE, vmx->msr_guest_kernel_gs_base);
#endif
	if (vmx->host_state.gs_ldt_reload_needed) {
		kvm_load_ldt(vmx->host_state.ldt_sel);
#ifdef CONFIG_X86_64
		load_gs_index(vmx->host_state.gs_sel);
#else
		loadsegment(gs, vmx->host_state.gs_sel);
#endif
	}
	if (vmx->host_state.fs_reload_needed)
		loadsegment(fs, vmx->host_state.fs_sel);
#ifdef CONFIG_X86_64
	if (unlikely(vmx->host_state.ds_sel | vmx->host_state.es_sel)) {
		loadsegment(ds, vmx->host_state.ds_sel);
		loadsegment(es, vmx->host_state.es_sel);
	}
#endif
	invalidate_tss_limit();
#ifdef CONFIG_X86_64
	wrmsrl(MSR_KERNEL_GS_BASE, vmx->msr_host_kernel_gs_base);
#endif
	if (vmx->host_state.msr_host_bndcfgs)
		wrmsrl(MSR_IA32_BNDCFGS, vmx->host_state.msr_host_bndcfgs);
	load_fixmap_gdt(raw_smp_processor_id());
}
