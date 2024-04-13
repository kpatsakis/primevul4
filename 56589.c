static int handle_pause(struct kvm_vcpu *vcpu)
{
	if (ple_gap)
		grow_ple_window(vcpu);

	/*
	 * Intel sdm vol3 ch-25.1.3 says: The "PAUSE-loop exiting"
	 * VM-execution control is ignored if CPL > 0. OTOH, KVM
	 * never set PAUSE_EXITING and just set PLE if supported,
	 * so the vcpu must be CPL=0 if it gets a PAUSE exit.
	 */
	kvm_vcpu_on_spin(vcpu, true);
	return kvm_skip_emulated_instruction(vcpu);
}
