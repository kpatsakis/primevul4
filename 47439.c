static s64 __kpit_elapsed(struct kvm *kvm)
{
	s64 elapsed;
	ktime_t remaining;
	struct kvm_kpit_state *ps = &kvm->arch.vpit->pit_state;

	if (!ps->pit_timer.period)
		return 0;

	/*
	 * The Counter does not stop when it reaches zero. In
	 * Modes 0, 1, 4, and 5 the Counter ``wraps around'' to
	 * the highest count, either FFFF hex for binary counting
	 * or 9999 for BCD counting, and continues counting.
	 * Modes 2 and 3 are periodic; the Counter reloads
	 * itself with the initial count and continues counting
	 * from there.
	 */
	remaining = hrtimer_get_remaining(&ps->pit_timer.timer);
	elapsed = ps->pit_timer.period - ktime_to_ns(remaining);
	elapsed = mod_64(elapsed, ps->pit_timer.period);

	return elapsed;
}
