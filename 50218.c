static bool rtc_irq_check_coalesced(struct kvm_ioapic *ioapic)
{
	if (ioapic->rtc_status.pending_eoi > 0)
		return true; /* coalesced */

	return false;
}
