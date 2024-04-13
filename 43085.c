static void toggle_interruptibility(struct kvm_vcpu *vcpu, u32 mask)
{
	u32 int_shadow = kvm_x86_ops->get_interrupt_shadow(vcpu);
	/*
	 * an sti; sti; sequence only disable interrupts for the first
	 * instruction. So, if the last instruction, be it emulated or
	 * not, left the system with the INT_STI flag enabled, it
	 * means that the last instruction is an sti. We should not
	 * leave the flag on in this case. The same goes for mov ss
	 */
	if (int_shadow & mask)
		mask = 0;
	if (unlikely(int_shadow || mask)) {
		kvm_x86_ops->set_interrupt_shadow(vcpu, mask);
		if (!mask)
			kvm_make_request(KVM_REQ_EVENT, vcpu);
	}
}
