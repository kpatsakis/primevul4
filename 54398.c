void kvm_notify_acked_irq(struct kvm *kvm, unsigned irqchip, unsigned pin)
{
	int gsi, idx;

	trace_kvm_ack_irq(irqchip, pin);

	idx = srcu_read_lock(&kvm->irq_srcu);
	gsi = kvm_irq_map_chip_pin(kvm, irqchip, pin);
	if (gsi != -1)
		kvm_notify_acked_gsi(kvm, gsi);
	srcu_read_unlock(&kvm->irq_srcu, idx);
}
