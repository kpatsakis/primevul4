static void kvm_ioapic_eoi_inject_work(struct work_struct *work)
{
	int i;
	struct kvm_ioapic *ioapic = container_of(work, struct kvm_ioapic,
						 eoi_inject.work);
	spin_lock(&ioapic->lock);
	for (i = 0; i < IOAPIC_NUM_PINS; i++) {
		union kvm_ioapic_redirect_entry *ent = &ioapic->redirtbl[i];

		if (ent->fields.trig_mode != IOAPIC_LEVEL_TRIG)
			continue;

		if (ioapic->irr & (1 << i) && !ent->fields.remote_irr)
			ioapic_service(ioapic, i, false);
	}
	spin_unlock(&ioapic->lock);
}
