static void __inject_pit_timer_intr(struct kvm *kvm)
{
	struct kvm_vcpu *vcpu;
	int i;

	kvm_set_irq(kvm, kvm->arch.vpit->irq_source_id, 0, 1);
	kvm_set_irq(kvm, kvm->arch.vpit->irq_source_id, 0, 0);

	/*
	 * Provides NMI watchdog support via Virtual Wire mode.
	 * The route is: PIT -> PIC -> LVT0 in NMI mode.
	 *
	 * Note: Our Virtual Wire implementation is simplified, only
	 * propagating PIT interrupts to all VCPUs when they have set
	 * LVT0 to NMI delivery. Other PIC interrupts are just sent to
	 * VCPU0, and only if its LVT0 is in EXTINT mode.
	 */
	if (kvm->arch.vapics_in_nmi_mode > 0)
		kvm_for_each_vcpu(i, vcpu, kvm)
			kvm_apic_nmi_wd_deliver(vcpu);
}
