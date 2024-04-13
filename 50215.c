static void kvm_rtc_eoi_tracking_restore_all(struct kvm_ioapic *ioapic)
{
	struct kvm_vcpu *vcpu;
	int i;

	if (RTC_GSI >= IOAPIC_NUM_PINS)
		return;

	rtc_irq_eoi_tracking_reset(ioapic);
	kvm_for_each_vcpu(i, vcpu, ioapic->kvm)
	    __rtc_irq_eoi_tracking_restore_one(vcpu);
}
