static int vcpu_enter_guest(struct kvm_vcpu *vcpu)
{
	int r;
	bool req_int_win = !irqchip_in_kernel(vcpu->kvm) &&
		vcpu->run->request_interrupt_window;

	if (vcpu->requests) {
		if (kvm_check_request(KVM_REQ_MMU_RELOAD, vcpu))
			kvm_mmu_unload(vcpu);
		if (kvm_check_request(KVM_REQ_MIGRATE_TIMER, vcpu))
			__kvm_migrate_timers(vcpu);
		if (kvm_check_request(KVM_REQ_CLOCK_UPDATE, vcpu)) {
			r = kvm_guest_time_update(vcpu);
			if (unlikely(r))
				goto out;
		}
		if (kvm_check_request(KVM_REQ_MMU_SYNC, vcpu))
			kvm_mmu_sync_roots(vcpu);
		if (kvm_check_request(KVM_REQ_TLB_FLUSH, vcpu))
			kvm_x86_ops->tlb_flush(vcpu);
		if (kvm_check_request(KVM_REQ_REPORT_TPR_ACCESS, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_TPR_ACCESS;
			r = 0;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_TRIPLE_FAULT, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_SHUTDOWN;
			r = 0;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_DEACTIVATE_FPU, vcpu)) {
			vcpu->fpu_active = 0;
			kvm_x86_ops->fpu_deactivate(vcpu);
		}
		if (kvm_check_request(KVM_REQ_APF_HALT, vcpu)) {
			/* Page is swapped out. Do synthetic halt */
			vcpu->arch.apf.halted = true;
			r = 1;
			goto out;
		}
	}

	r = kvm_mmu_reload(vcpu);
	if (unlikely(r))
		goto out;

	if (kvm_check_request(KVM_REQ_EVENT, vcpu) || req_int_win) {
		inject_pending_event(vcpu);

		/* enable NMI/IRQ window open exits if needed */
		if (vcpu->arch.nmi_pending)
			kvm_x86_ops->enable_nmi_window(vcpu);
		else if (kvm_cpu_has_interrupt(vcpu) || req_int_win)
			kvm_x86_ops->enable_irq_window(vcpu);

		if (kvm_lapic_enabled(vcpu)) {
			update_cr8_intercept(vcpu);
			kvm_lapic_sync_to_vapic(vcpu);
		}
	}

	preempt_disable();

	kvm_x86_ops->prepare_guest_switch(vcpu);
	if (vcpu->fpu_active)
		kvm_load_guest_fpu(vcpu);
	kvm_load_guest_xcr0(vcpu);

	atomic_set(&vcpu->guest_mode, 1);
	smp_wmb();

	local_irq_disable();

	if (!atomic_read(&vcpu->guest_mode) || vcpu->requests
	    || need_resched() || signal_pending(current)) {
		atomic_set(&vcpu->guest_mode, 0);
		smp_wmb();
		local_irq_enable();
		preempt_enable();
		kvm_x86_ops->cancel_injection(vcpu);
		r = 1;
		goto out;
	}

	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);

	kvm_guest_enter();

	if (unlikely(vcpu->arch.switch_db_regs)) {
		set_debugreg(0, 7);
		set_debugreg(vcpu->arch.eff_db[0], 0);
		set_debugreg(vcpu->arch.eff_db[1], 1);
		set_debugreg(vcpu->arch.eff_db[2], 2);
		set_debugreg(vcpu->arch.eff_db[3], 3);
	}

	trace_kvm_entry(vcpu->vcpu_id);
	kvm_x86_ops->run(vcpu);

	/*
	 * If the guest has used debug registers, at least dr7
	 * will be disabled while returning to the host.
	 * If we don't have active breakpoints in the host, we don't
	 * care about the messed up debug address registers. But if
	 * we have some of them active, restore the old state.
	 */
	if (hw_breakpoint_active())
		hw_breakpoint_restore();

	kvm_get_msr(vcpu, MSR_IA32_TSC, &vcpu->arch.last_guest_tsc);

	atomic_set(&vcpu->guest_mode, 0);
	smp_wmb();
	local_irq_enable();

	++vcpu->stat.exits;

	/*
	 * We must have an instruction between local_irq_enable() and
	 * kvm_guest_exit(), so the timer interrupt isn't delayed by
	 * the interrupt shadow.  The stat.exits increment will do nicely.
	 * But we need to prevent reordering, hence this barrier():
	 */
	barrier();

	kvm_guest_exit();

	preempt_enable();

	vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);

	/*
	 * Profile KVM exit RIPs:
	 */
	if (unlikely(prof_on == KVM_PROFILING)) {
		unsigned long rip = kvm_rip_read(vcpu);
		profile_hit(KVM_PROFILING, (void *)rip);
	}


	kvm_lapic_sync_from_vapic(vcpu);

	r = kvm_x86_ops->handle_exit(vcpu);
out:
	return r;
}
