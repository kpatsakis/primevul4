static int kvm_guest_time_update(struct kvm_vcpu *v)
{
	unsigned long flags, this_tsc_khz, tgt_tsc_khz;
	struct kvm_vcpu_arch *vcpu = &v->arch;
	struct kvm_arch *ka = &v->kvm->arch;
	s64 kernel_ns;
	u64 tsc_timestamp, host_tsc;
	struct pvclock_vcpu_time_info guest_hv_clock;
	u8 pvclock_flags;
	bool use_master_clock;

	kernel_ns = 0;
	host_tsc = 0;

	/*
	 * If the host uses TSC clock, then passthrough TSC as stable
	 * to the guest.
	 */
	spin_lock(&ka->pvclock_gtod_sync_lock);
	use_master_clock = ka->use_master_clock;
	if (use_master_clock) {
		host_tsc = ka->master_cycle_now;
		kernel_ns = ka->master_kernel_ns;
	}
	spin_unlock(&ka->pvclock_gtod_sync_lock);

	/* Keep irq disabled to prevent changes to the clock */
	local_irq_save(flags);
	this_tsc_khz = __this_cpu_read(cpu_tsc_khz);
	if (unlikely(this_tsc_khz == 0)) {
		local_irq_restore(flags);
		kvm_make_request(KVM_REQ_CLOCK_UPDATE, v);
		return 1;
	}
	if (!use_master_clock) {
		host_tsc = rdtsc();
		kernel_ns = get_kernel_ns();
	}

	tsc_timestamp = kvm_read_l1_tsc(v, host_tsc);

	/*
	 * We may have to catch up the TSC to match elapsed wall clock
	 * time for two reasons, even if kvmclock is used.
	 *   1) CPU could have been running below the maximum TSC rate
	 *   2) Broken TSC compensation resets the base at each VCPU
	 *      entry to avoid unknown leaps of TSC even when running
	 *      again on the same CPU.  This may cause apparent elapsed
	 *      time to disappear, and the guest to stand still or run
	 *	very slowly.
	 */
	if (vcpu->tsc_catchup) {
		u64 tsc = compute_guest_tsc(v, kernel_ns);
		if (tsc > tsc_timestamp) {
			adjust_tsc_offset_guest(v, tsc - tsc_timestamp);
			tsc_timestamp = tsc;
		}
	}

	local_irq_restore(flags);

	if (!vcpu->pv_time_enabled)
		return 0;

	if (unlikely(vcpu->hw_tsc_khz != this_tsc_khz)) {
		tgt_tsc_khz = kvm_has_tsc_control ?
			vcpu->virtual_tsc_khz : this_tsc_khz;
		kvm_get_time_scale(NSEC_PER_SEC / 1000, tgt_tsc_khz,
				   &vcpu->hv_clock.tsc_shift,
				   &vcpu->hv_clock.tsc_to_system_mul);
		vcpu->hw_tsc_khz = this_tsc_khz;
	}

	/* With all the info we got, fill in the values */
	vcpu->hv_clock.tsc_timestamp = tsc_timestamp;
	vcpu->hv_clock.system_time = kernel_ns + v->kvm->arch.kvmclock_offset;
	vcpu->last_guest_tsc = tsc_timestamp;

	if (unlikely(kvm_read_guest_cached(v->kvm, &vcpu->pv_time,
		&guest_hv_clock, sizeof(guest_hv_clock))))
		return 0;

	/* This VCPU is paused, but it's legal for a guest to read another
	 * VCPU's kvmclock, so we really have to follow the specification where
	 * it says that version is odd if data is being modified, and even after
	 * it is consistent.
	 *
	 * Version field updates must be kept separate.  This is because
	 * kvm_write_guest_cached might use a "rep movs" instruction, and
	 * writes within a string instruction are weakly ordered.  So there
	 * are three writes overall.
	 *
	 * As a small optimization, only write the version field in the first
	 * and third write.  The vcpu->pv_time cache is still valid, because the
	 * version field is the first in the struct.
	 */
	BUILD_BUG_ON(offsetof(struct pvclock_vcpu_time_info, version) != 0);

	vcpu->hv_clock.version = guest_hv_clock.version + 1;
	kvm_write_guest_cached(v->kvm, &vcpu->pv_time,
				&vcpu->hv_clock,
				sizeof(vcpu->hv_clock.version));

	smp_wmb();

	/* retain PVCLOCK_GUEST_STOPPED if set in guest copy */
	pvclock_flags = (guest_hv_clock.flags & PVCLOCK_GUEST_STOPPED);

	if (vcpu->pvclock_set_guest_stopped_request) {
		pvclock_flags |= PVCLOCK_GUEST_STOPPED;
		vcpu->pvclock_set_guest_stopped_request = false;
	}

	/* If the host uses TSC clocksource, then it is stable */
	if (use_master_clock)
		pvclock_flags |= PVCLOCK_TSC_STABLE_BIT;

	vcpu->hv_clock.flags = pvclock_flags;

	trace_kvm_pvclock_update(v->vcpu_id, &vcpu->hv_clock);

	kvm_write_guest_cached(v->kvm, &vcpu->pv_time,
				&vcpu->hv_clock,
				sizeof(vcpu->hv_clock));

	smp_wmb();

	vcpu->hv_clock.version++;
	kvm_write_guest_cached(v->kvm, &vcpu->pv_time,
				&vcpu->hv_clock,
				sizeof(vcpu->hv_clock.version));
	return 0;
}
