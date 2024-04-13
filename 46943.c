static void vmx_vcpu_pi_load(struct kvm_vcpu *vcpu, int cpu)
{
	struct pi_desc *pi_desc = vcpu_to_pi_desc(vcpu);
	struct pi_desc old, new;
	unsigned int dest;

	if (!kvm_arch_has_assigned_device(vcpu->kvm) ||
		!irq_remapping_cap(IRQ_POSTING_CAP))
		return;

	do {
		old.control = new.control = pi_desc->control;

		/*
		 * If 'nv' field is POSTED_INTR_WAKEUP_VECTOR, there
		 * are two possible cases:
		 * 1. After running 'pre_block', context switch
		 *    happened. For this case, 'sn' was set in
		 *    vmx_vcpu_put(), so we need to clear it here.
		 * 2. After running 'pre_block', we were blocked,
		 *    and woken up by some other guy. For this case,
		 *    we don't need to do anything, 'pi_post_block'
		 *    will do everything for us. However, we cannot
		 *    check whether it is case #1 or case #2 here
		 *    (maybe, not needed), so we also clear sn here,
		 *    I think it is not a big deal.
		 */
		if (pi_desc->nv != POSTED_INTR_WAKEUP_VECTOR) {
			if (vcpu->cpu != cpu) {
				dest = cpu_physical_id(cpu);

				if (x2apic_enabled())
					new.ndst = dest;
				else
					new.ndst = (dest << 8) & 0xFF00;
			}

			/* set 'NV' to 'notification vector' */
			new.nv = POSTED_INTR_VECTOR;
		}

		/* Allow posting non-urgent interrupts */
		new.sn = 0;
	} while (cmpxchg(&pi_desc->control, old.control,
			new.control) != old.control);
}
