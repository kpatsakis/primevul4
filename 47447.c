void kvm_pit_reset(struct kvm_pit *pit)
{
	int i;
	struct kvm_kpit_channel_state *c;

	mutex_lock(&pit->pit_state.lock);
	pit->pit_state.flags = 0;
	for (i = 0; i < 3; i++) {
		c = &pit->pit_state.channels[i];
		c->mode = 0xff;
		c->gate = (i != 2);
		pit_load_count(pit->kvm, i, 0);
	}
	mutex_unlock(&pit->pit_state.lock);

	atomic_set(&pit->pit_state.pit_timer.pending, 0);
	pit->pit_state.irq_ack = 1;
}
