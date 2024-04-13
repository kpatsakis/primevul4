static void vmx_post_block(struct kvm_vcpu *vcpu)
{
	struct pi_desc *pi_desc = vcpu_to_pi_desc(vcpu);
	struct pi_desc old, new;
	unsigned int dest;
	unsigned long flags;

	if (!kvm_arch_has_assigned_device(vcpu->kvm) ||
		!irq_remapping_cap(IRQ_POSTING_CAP))
		return;

	do {
		old.control = new.control = pi_desc->control;

		dest = cpu_physical_id(vcpu->cpu);

		if (x2apic_enabled())
			new.ndst = dest;
		else
			new.ndst = (dest << 8) & 0xFF00;

		/* Allow posting non-urgent interrupts */
		new.sn = 0;

		/* set 'NV' to 'notification vector' */
		new.nv = POSTED_INTR_VECTOR;
	} while (cmpxchg(&pi_desc->control, old.control,
			new.control) != old.control);

	if(vcpu->pre_pcpu != -1) {
		spin_lock_irqsave(
			&per_cpu(blocked_vcpu_on_cpu_lock,
			vcpu->pre_pcpu), flags);
		list_del(&vcpu->blocked_vcpu_list);
		spin_unlock_irqrestore(
			&per_cpu(blocked_vcpu_on_cpu_lock,
			vcpu->pre_pcpu), flags);
		vcpu->pre_pcpu = -1;
	}
}
