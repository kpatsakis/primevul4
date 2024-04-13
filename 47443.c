static bool kpit_is_periodic(struct kvm_timer *ktimer)
{
	struct kvm_kpit_state *ps = container_of(ktimer, struct kvm_kpit_state,
						 pit_timer);
	return ps->is_periodic;
}
