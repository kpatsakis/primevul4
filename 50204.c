static int ioapic_set_irq(struct kvm_ioapic *ioapic, unsigned int irq,
		int irq_level, bool line_status)
{
	union kvm_ioapic_redirect_entry entry;
	u32 mask = 1 << irq;
	u32 old_irr;
	int edge, ret;

	entry = ioapic->redirtbl[irq];
	edge = (entry.fields.trig_mode == IOAPIC_EDGE_TRIG);

	if (!irq_level) {
		ioapic->irr &= ~mask;
		ret = 1;
		goto out;
	}

	/*
	 * Return 0 for coalesced interrupts; for edge-triggered interrupts,
	 * this only happens if a previous edge has not been delivered due
	 * do masking.  For level interrupts, the remote_irr field tells
	 * us if the interrupt is waiting for an EOI.
	 *
	 * RTC is special: it is edge-triggered, but userspace likes to know
	 * if it has been already ack-ed via EOI because coalesced RTC
	 * interrupts lead to time drift in Windows guests.  So we track
	 * EOI manually for the RTC interrupt.
	 */
	if (irq == RTC_GSI && line_status &&
		rtc_irq_check_coalesced(ioapic)) {
		ret = 0;
		goto out;
	}

	old_irr = ioapic->irr;
	ioapic->irr |= mask;
	if (edge)
		ioapic->irr_delivered &= ~mask;
	if ((edge && old_irr == ioapic->irr) ||
	    (!edge && entry.fields.remote_irr)) {
		ret = 0;
		goto out;
	}

	ret = ioapic_service(ioapic, irq, line_status);

out:
	trace_kvm_ioapic_set_irq(entry.bits, irq, ret == 0);
	return ret;
}
