static void pit_mask_notifer(struct kvm_irq_mask_notifier *kimn, bool mask)
{
	struct kvm_pit *pit = container_of(kimn, struct kvm_pit, mask_notifier);

	if (!mask) {
		atomic_set(&pit->pit_state.pit_timer.pending, 0);
		pit->pit_state.irq_ack = 1;
	}
}
