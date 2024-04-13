static int vmx_pre_block(struct kvm_vcpu *vcpu)
{
	unsigned long flags;
	unsigned int dest;
	struct pi_desc old, new;
	struct pi_desc *pi_desc = vcpu_to_pi_desc(vcpu);

	if (!kvm_arch_has_assigned_device(vcpu->kvm) ||
		!irq_remapping_cap(IRQ_POSTING_CAP))
		return 0;

	vcpu->pre_pcpu = vcpu->cpu;
	spin_lock_irqsave(&per_cpu(blocked_vcpu_on_cpu_lock,
			  vcpu->pre_pcpu), flags);
	list_add_tail(&vcpu->blocked_vcpu_list,
		      &per_cpu(blocked_vcpu_on_cpu,
		      vcpu->pre_pcpu));
	spin_unlock_irqrestore(&per_cpu(blocked_vcpu_on_cpu_lock,
			       vcpu->pre_pcpu), flags);

	do {
		old.control = new.control = pi_desc->control;

		/*
		 * We should not block the vCPU if
		 * an interrupt is posted for it.
		 */
		if (pi_test_on(pi_desc) == 1) {
			spin_lock_irqsave(&per_cpu(blocked_vcpu_on_cpu_lock,
					  vcpu->pre_pcpu), flags);
			list_del(&vcpu->blocked_vcpu_list);
			spin_unlock_irqrestore(
					&per_cpu(blocked_vcpu_on_cpu_lock,
					vcpu->pre_pcpu), flags);
			vcpu->pre_pcpu = -1;

			return 1;
		}

		WARN((pi_desc->sn == 1),
		     "Warning: SN field of posted-interrupts "
		     "is set before blocking\n");

		/*
		 * Since vCPU can be preempted during this process,
		 * vcpu->cpu could be different with pre_pcpu, we
		 * need to set pre_pcpu as the destination of wakeup
		 * notification event, then we can find the right vCPU
		 * to wakeup in wakeup handler if interrupts happen
		 * when the vCPU is in blocked state.
		 */
		dest = cpu_physical_id(vcpu->pre_pcpu);

		if (x2apic_enabled())
			new.ndst = dest;
		else
			new.ndst = (dest << 8) & 0xFF00;

		/* set 'NV' to 'wakeup vector' */
		new.nv = POSTED_INTR_WAKEUP_VECTOR;
	} while (cmpxchg(&pi_desc->control, old.control,
			new.control) != old.control);

	return 0;
}
