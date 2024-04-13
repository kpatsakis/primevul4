int kvm_ioapic_set_irq(struct kvm_ioapic *ioapic, int irq, int irq_source_id,
		       int level, bool line_status)
{
	int ret, irq_level;

	BUG_ON(irq < 0 || irq >= IOAPIC_NUM_PINS);

	spin_lock(&ioapic->lock);
	irq_level = __kvm_irq_line_state(&ioapic->irq_states[irq],
					 irq_source_id, level);
	ret = ioapic_set_irq(ioapic, irq, irq_level, line_status);

	spin_unlock(&ioapic->lock);

	return ret;
}
