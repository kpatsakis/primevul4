static int vcpu_enter_guest(struct kvm_vcpu *vcpu)
{
	int r;
	bool req_int_win = !irqchip_in_kernel(vcpu->kvm) &&
		vcpu->run->request_interrupt_window;
	bool req_immediate_exit = false;

	if (vcpu->requests) {
		if (kvm_check_request(KVM_REQ_MMU_RELOAD, vcpu))
			kvm_mmu_unload(vcpu);
		if (kvm_check_request(KVM_REQ_MIGRATE_TIMER, vcpu))
			__kvm_migrate_timers(vcpu);
		if (kvm_check_request(KVM_REQ_MASTERCLOCK_UPDATE, vcpu))
			kvm_gen_update_masterclock(vcpu->kvm);
		if (kvm_check_request(KVM_REQ_GLOBAL_CLOCK_UPDATE, vcpu))
			kvm_gen_kvmclock_update(vcpu);
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
		if (kvm_check_request(KVM_REQ_STEAL_UPDATE, vcpu))
			record_steal_time(vcpu);
		if (kvm_check_request(KVM_REQ_NMI, vcpu))
			process_nmi(vcpu);
		if (kvm_check_request(KVM_REQ_PMU, vcpu))
			kvm_handle_pmu_event(vcpu);
		if (kvm_check_request(KVM_REQ_PMI, vcpu))
			kvm_deliver_pmi(vcpu);
		if (kvm_check_request(KVM_REQ_SCAN_IOAPIC, vcpu))
			vcpu_scan_ioapic(vcpu);
	}

	if (kvm_check_request(KVM_REQ_EVENT, vcpu) || req_int_win) {
		kvm_apic_accept_events(vcpu);
		if (vcpu->arch.mp_state == KVM_MP_STATE_INIT_RECEIVED) {
			r = 1;
			goto out;
		}

		if (inject_pending_event(vcpu, req_int_win) != 0)
			req_immediate_exit = true;
		/* enable NMI/IRQ window open exits if needed */
		else if (vcpu->arch.nmi_pending)
			kvm_x86_ops->enable_nmi_window(vcpu);
		else if (kvm_cpu_has_injectable_intr(vcpu) || req_int_win)
			kvm_x86_ops->enable_irq_window(vcpu);

		if (kvm_lapic_enabled(vcpu)) {
			/*
			 * Update architecture specific hints for APIC
			 * virtual interrupt delivery.
			 */
			if (kvm_x86_ops->hwapic_irr_update)
				kvm_x86_ops->hwapic_irr_update(vcpu,
					kvm_lapic_find_highest_irr(vcpu));
			update_cr8_intercept(vcpu);
			kvm_lapic_sync_to_vapic(vcpu);
		}
	}

	r = kvm_mmu_reload(vcpu);
	if (unlikely(r)) {
		goto cancel_injection;
	}

	preempt_disable();

	kvm_x86_ops->prepare_guest_switch(vcpu);
	if (vcpu->fpu_active)
		kvm_load_guest_fpu(vcpu);
	kvm_load_guest_xcr0(vcpu);

	vcpu->mode = IN_GUEST_MODE;

	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);

	/* We should set ->mode before check ->requests,
	 * see the comment in make_all_cpus_request.
	 */
	smp_mb__after_srcu_read_unlock();

	local_irq_disable();

	if (vcpu->mode == EXITING_GUEST_MODE || vcpu->requests
	    || need_resched() || signal_pending(current)) {
		vcpu->mode = OUTSIDE_GUEST_MODE;
		smp_wmb();
		local_irq_enable();
		preempt_enable();
		vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);
		r = 1;
		goto cancel_injection;
	}

	if (req_immediate_exit)
		smp_send_reschedule(vcpu->cpu);

	kvm_guest_enter();

	if (unlikely(vcpu->arch.switch_db_regs)) {
		set_debugreg(0, 7);
		set_debugreg(vcpu->arch.eff_db[0], 0);
		set_debugreg(vcpu->arch.eff_db[1], 1);
		set_debugreg(vcpu->arch.eff_db[2], 2);
		set_debugreg(vcpu->arch.eff_db[3], 3);
		set_debugreg(vcpu->arch.dr6, 6);
	}

	trace_kvm_entry(vcpu->vcpu_id);
	kvm_x86_ops->run(vcpu);

	/*
	 * Do this here before restoring debug registers on the host.  And
	 * since we do this before handling the vmexit, a DR access vmexit
	 * can (a) read the correct value of the debug registers, (b) set
	 * KVM_DEBUGREG_WONT_EXIT again.
	 */
	if (unlikely(vcpu->arch.switch_db_regs & KVM_DEBUGREG_WONT_EXIT)) {
		int i;

		WARN_ON(vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP);
		kvm_x86_ops->sync_dirty_debug_regs(vcpu);
		for (i = 0; i < KVM_NR_DB_REGS; i++)
			vcpu->arch.eff_db[i] = vcpu->arch.db[i];
	}

	/*
	 * If the guest has used debug registers, at least dr7
	 * will be disabled while returning to the host.
	 * If we don't have active breakpoints in the host, we don't
	 * care about the messed up debug address registers. But if
	 * we have some of them active, restore the old state.
	 */
	if (hw_breakpoint_active())
		hw_breakpoint_restore();

	vcpu->arch.last_guest_tsc = kvm_x86_ops->read_l1_tsc(vcpu,
							   native_read_tsc());

	vcpu->mode = OUTSIDE_GUEST_MODE;
	smp_wmb();

	/* Interrupt is enabled by handle_external_intr() */
	kvm_x86_ops->handle_external_intr(vcpu);

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

	if (unlikely(vcpu->arch.tsc_always_catchup))
		kvm_make_request(KVM_REQ_CLOCK_UPDATE, vcpu);

	if (vcpu->arch.apic_attention)
		kvm_lapic_sync_from_vapic(vcpu);

	r = kvm_x86_ops->handle_exit(vcpu);
	return r;

cancel_injection:
	kvm_x86_ops->cancel_injection(vcpu);
	if (unlikely(vcpu->arch.apic_attention))
		kvm_lapic_sync_from_vapic(vcpu);
out:
	return r;
}
