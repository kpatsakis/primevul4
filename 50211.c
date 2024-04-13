static void kvm_ioapic_reset(struct kvm_ioapic *ioapic)
{
	int i;

	cancel_delayed_work_sync(&ioapic->eoi_inject);
	for (i = 0; i < IOAPIC_NUM_PINS; i++)
		ioapic->redirtbl[i].fields.mask = 1;
	ioapic->base_address = IOAPIC_DEFAULT_BASE_ADDRESS;
	ioapic->ioregsel = 0;
	ioapic->irr = 0;
	ioapic->irr_delivered = 0;
	ioapic->id = 0;
	memset(ioapic->irq_eoi, 0x00, sizeof(ioapic->irq_eoi));
	rtc_irq_eoi_tracking_reset(ioapic);
}
