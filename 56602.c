static inline bool kvm_vcpu_trigger_posted_interrupt(struct kvm_vcpu *vcpu,
						     bool nested)
{
#ifdef CONFIG_SMP
	int pi_vec = nested ? POSTED_INTR_NESTED_VECTOR : POSTED_INTR_VECTOR;

	if (vcpu->mode == IN_GUEST_MODE) {
		struct vcpu_vmx *vmx = to_vmx(vcpu);

		/*
		 * Currently, we don't support urgent interrupt,
		 * all interrupts are recognized as non-urgent
		 * interrupt, so we cannot post interrupts when
		 * 'SN' is set.
		 *
		 * If the vcpu is in guest mode, it means it is
		 * running instead of being scheduled out and
		 * waiting in the run queue, and that's the only
		 * case when 'SN' is set currently, warning if
		 * 'SN' is set.
		 */
		WARN_ON_ONCE(pi_test_sn(&vmx->pi_desc));

		apic->send_IPI_mask(get_cpu_mask(vcpu->cpu), pi_vec);
		return true;
	}
#endif
	return false;
}
