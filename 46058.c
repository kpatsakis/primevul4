void kvm_arch_vcpu_load(struct kvm_vcpu *vcpu, int cpu)
{
	/* Address WBINVD may be executed by guest */
	if (need_emulate_wbinvd(vcpu)) {
		if (kvm_x86_ops->has_wbinvd_exit())
			cpumask_set_cpu(cpu, vcpu->arch.wbinvd_dirty_mask);
		else if (vcpu->cpu != -1 && vcpu->cpu != cpu)
			smp_call_function_single(vcpu->cpu,
					wbinvd_ipi, NULL, 1);
	}

	kvm_x86_ops->vcpu_load(vcpu, cpu);
	if (unlikely(vcpu->cpu != cpu) || check_tsc_unstable()) {
		/* Make sure TSC doesn't go backwards */
		s64 tsc_delta = !vcpu->arch.last_host_tsc ? 0 :
				native_read_tsc() - vcpu->arch.last_host_tsc;
		if (tsc_delta < 0)
			mark_tsc_unstable("KVM discovered backwards TSC");
		if (check_tsc_unstable()) {
			kvm_x86_ops->adjust_tsc_offset(vcpu, -tsc_delta);
			vcpu->arch.tsc_catchup = 1;
			kvm_make_request(KVM_REQ_CLOCK_UPDATE, vcpu);
		}
		if (vcpu->cpu != cpu)
			kvm_migrate_timers(vcpu);
		vcpu->cpu = cpu;
	}
}
