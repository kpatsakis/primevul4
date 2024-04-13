void kvm_write_tsc(struct kvm_vcpu *vcpu, u64 data)
{
	struct kvm *kvm = vcpu->kvm;
	u64 offset, ns, elapsed;
	unsigned long flags;
	s64 sdiff;

	spin_lock_irqsave(&kvm->arch.tsc_write_lock, flags);
	offset = data - native_read_tsc();
	ns = get_kernel_ns();
	elapsed = ns - kvm->arch.last_tsc_nsec;
	sdiff = data - kvm->arch.last_tsc_write;
	if (sdiff < 0)
		sdiff = -sdiff;

	/*
	 * Special case: close write to TSC within 5 seconds of
	 * another CPU is interpreted as an attempt to synchronize
	 * The 5 seconds is to accomodate host load / swapping as
	 * well as any reset of TSC during the boot process.
	 *
	 * In that case, for a reliable TSC, we can match TSC offsets,
	 * or make a best guest using elapsed value.
	 */
	if (sdiff < nsec_to_cycles(5ULL * NSEC_PER_SEC) &&
	    elapsed < 5ULL * NSEC_PER_SEC) {
		if (!check_tsc_unstable()) {
			offset = kvm->arch.last_tsc_offset;
			pr_debug("kvm: matched tsc offset for %llu\n", data);
		} else {
			u64 delta = nsec_to_cycles(elapsed);
			offset += delta;
			pr_debug("kvm: adjusted tsc offset by %llu\n", delta);
		}
		ns = kvm->arch.last_tsc_nsec;
	}
	kvm->arch.last_tsc_nsec = ns;
	kvm->arch.last_tsc_write = data;
	kvm->arch.last_tsc_offset = offset;
	kvm_x86_ops->write_tsc_offset(vcpu, offset);
	spin_unlock_irqrestore(&kvm->arch.tsc_write_lock, flags);

	/* Reset of TSC must disable overshoot protection below */
	vcpu->arch.hv_clock.tsc_timestamp = 0;
	vcpu->arch.last_tsc_write = data;
	vcpu->arch.last_tsc_nsec = ns;
}
