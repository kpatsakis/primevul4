static void create_pit_timer(struct kvm_kpit_state *ps, u32 val, int is_period)
{
	struct kvm_timer *pt = &ps->pit_timer;
	s64 interval;

	interval = muldiv64(val, NSEC_PER_SEC, KVM_PIT_FREQ);

	pr_debug("create pit timer, interval is %llu nsec\n", interval);

	/* TODO The new value only affected after the retriggered */
	hrtimer_cancel(&pt->timer);
	pt->period = interval;
	ps->is_periodic = is_period;

	pt->timer.function = kvm_timer_fn;
	pt->t_ops = &kpit_ops;
	pt->kvm = ps->pit->kvm;
	pt->vcpu = pt->kvm->bsp_vcpu;

	atomic_set(&pt->pending, 0);
	ps->irq_ack = 1;

	hrtimer_start(&pt->timer, ktime_add_ns(ktime_get(), interval),
		      HRTIMER_MODE_ABS);
}
