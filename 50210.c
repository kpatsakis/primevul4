static void kvm_ioapic_inject_all(struct kvm_ioapic *ioapic, unsigned long irr)
{
	u32 idx;

	rtc_irq_eoi_tracking_reset(ioapic);
	for_each_set_bit(idx, &irr, IOAPIC_NUM_PINS)
		ioapic_set_irq(ioapic, idx, 1, true);

	kvm_rtc_eoi_tracking_restore_all(ioapic);
}
