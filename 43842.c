static void add_atomic_switch_msr(struct vcpu_vmx *vmx, unsigned msr,
				  u64 guest_val, u64 host_val)
{
	unsigned i;
	struct msr_autoload *m = &vmx->msr_autoload;

	switch (msr) {
	case MSR_EFER:
		if (cpu_has_load_ia32_efer) {
			add_atomic_switch_msr_special(VM_ENTRY_LOAD_IA32_EFER,
					VM_EXIT_LOAD_IA32_EFER,
					GUEST_IA32_EFER,
					HOST_IA32_EFER,
					guest_val, host_val);
			return;
		}
		break;
	case MSR_CORE_PERF_GLOBAL_CTRL:
		if (cpu_has_load_perf_global_ctrl) {
			add_atomic_switch_msr_special(
					VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL,
					VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL,
					GUEST_IA32_PERF_GLOBAL_CTRL,
					HOST_IA32_PERF_GLOBAL_CTRL,
					guest_val, host_val);
			return;
		}
		break;
	}

	for (i = 0; i < m->nr; ++i)
		if (m->guest[i].index == msr)
			break;

	if (i == NR_AUTOLOAD_MSRS) {
		printk_once(KERN_WARNING"Not enough mst switch entries. "
				"Can't add msr %x\n", msr);
		return;
	} else if (i == m->nr) {
		++m->nr;
		vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, m->nr);
		vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, m->nr);
	}

	m->guest[i].index = msr;
	m->guest[i].value = guest_val;
	m->host[i].index = msr;
	m->host[i].value = host_val;
}
