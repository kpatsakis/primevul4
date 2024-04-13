static void __kvm_ioapic_update_eoi(struct kvm_vcpu *vcpu,
			struct kvm_ioapic *ioapic, int vector, int trigger_mode)
{
	struct dest_map *dest_map = &ioapic->rtc_status.dest_map;
	struct kvm_lapic *apic = vcpu->arch.apic;
	int i;

	/* RTC special handling */
	if (test_bit(vcpu->vcpu_id, dest_map->map) &&
	    vector == dest_map->vectors[vcpu->vcpu_id])
		rtc_irq_eoi(ioapic, vcpu);

	for (i = 0; i < IOAPIC_NUM_PINS; i++) {
		union kvm_ioapic_redirect_entry *ent = &ioapic->redirtbl[i];

		if (ent->fields.vector != vector)
			continue;

		/*
		 * We are dropping lock while calling ack notifiers because ack
		 * notifier callbacks for assigned devices call into IOAPIC
		 * recursively. Since remote_irr is cleared only after call
		 * to notifiers if the same vector will be delivered while lock
		 * is dropped it will be put into irr and will be delivered
		 * after ack notifier returns.
		 */
		spin_unlock(&ioapic->lock);
		kvm_notify_acked_irq(ioapic->kvm, KVM_IRQCHIP_IOAPIC, i);
		spin_lock(&ioapic->lock);

		if (trigger_mode != IOAPIC_LEVEL_TRIG ||
		    kvm_lapic_get_reg(apic, APIC_SPIV) & APIC_SPIV_DIRECTED_EOI)
			continue;

		ASSERT(ent->fields.trig_mode == IOAPIC_LEVEL_TRIG);
		ent->fields.remote_irr = 0;
		if (!ent->fields.mask && (ioapic->irr & (1 << i))) {
			++ioapic->irq_eoi[i];
			if (ioapic->irq_eoi[i] == IOAPIC_SUCCESSIVE_IRQ_MAX_COUNT) {
				/*
				 * Real hardware does not deliver the interrupt
				 * immediately during eoi broadcast, and this
				 * lets a buggy guest make slow progress
				 * even if it does not correctly handle a
				 * level-triggered interrupt.  Emulate this
				 * behavior if we detect an interrupt storm.
				 */
				schedule_delayed_work(&ioapic->eoi_inject, HZ / 100);
				ioapic->irq_eoi[i] = 0;
				trace_kvm_ioapic_delayed_eoi_inj(ent->bits);
			} else {
				ioapic_service(ioapic, i, false);
			}
		} else {
			ioapic->irq_eoi[i] = 0;
		}
	}
}
