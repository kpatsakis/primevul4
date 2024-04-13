static void rtc_status_pending_eoi_check_valid(struct kvm_ioapic *ioapic)
{
	if (WARN_ON(ioapic->rtc_status.pending_eoi < 0))
		kvm_rtc_eoi_tracking_restore_all(ioapic);
}
