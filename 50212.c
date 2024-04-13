void kvm_ioapic_scan_entry(struct kvm_vcpu *vcpu, ulong *ioapic_handled_vectors)
{
	struct kvm_ioapic *ioapic = vcpu->kvm->arch.vioapic;
	struct dest_map *dest_map = &ioapic->rtc_status.dest_map;
	union kvm_ioapic_redirect_entry *e;
	int index;

	spin_lock(&ioapic->lock);

	/* Make sure we see any missing RTC EOI */
	if (test_bit(vcpu->vcpu_id, dest_map->map))
		__set_bit(dest_map->vectors[vcpu->vcpu_id],
			  ioapic_handled_vectors);

	for (index = 0; index < IOAPIC_NUM_PINS; index++) {
		e = &ioapic->redirtbl[index];
		if (e->fields.trig_mode == IOAPIC_LEVEL_TRIG ||
		    kvm_irq_has_notifier(ioapic->kvm, KVM_IRQCHIP_IOAPIC, index) ||
		    index == RTC_GSI) {
			if (kvm_apic_match_dest(vcpu, NULL, 0,
			             e->fields.dest_id, e->fields.dest_mode) ||
			    (e->fields.trig_mode == IOAPIC_EDGE_TRIG &&
			     kvm_apic_pending_eoi(vcpu, e->fields.vector)))
				__set_bit(e->fields.vector,
					  ioapic_handled_vectors);
		}
	}
	spin_unlock(&ioapic->lock);
}
