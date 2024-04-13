static void pit_load_count(struct kvm *kvm, int channel, u32 val)
{
	struct kvm_kpit_state *ps = &kvm->arch.vpit->pit_state;

	WARN_ON(!mutex_is_locked(&ps->lock));

	pr_debug("load_count val is %d, channel is %d\n", val, channel);

	/*
	 * The largest possible initial count is 0; this is equivalent
	 * to 216 for binary counting and 104 for BCD counting.
	 */
	if (val == 0)
		val = 0x10000;

	ps->channels[channel].count = val;

	if (channel != 0) {
		ps->channels[channel].count_load_time = ktime_get();
		return;
	}

	/* Two types of timer
	 * mode 1 is one shot, mode 2 is period, otherwise del timer */
	switch (ps->channels[0].mode) {
	case 0:
	case 1:
        /* FIXME: enhance mode 4 precision */
	case 4:
		if (!(ps->flags & KVM_PIT_FLAGS_HPET_LEGACY)) {
			create_pit_timer(ps, val, 0);
		}
		break;
	case 2:
	case 3:
		if (!(ps->flags & KVM_PIT_FLAGS_HPET_LEGACY)){
			create_pit_timer(ps, val, 1);
		}
		break;
	default:
		destroy_pit_timer(&ps->pit_timer);
	}
}
