static void destroy_pit_timer(struct kvm_timer *pt)
{
	pr_debug("execute del timer!\n");
	hrtimer_cancel(&pt->timer);
}
