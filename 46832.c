static void fix_pmode_seg(struct kvm_vcpu *vcpu, int seg,
		struct kvm_segment *save)
{
	if (!emulate_invalid_guest_state) {
		/*
		 * CS and SS RPL should be equal during guest entry according
		 * to VMX spec, but in reality it is not always so. Since vcpu
		 * is in the middle of the transition from real mode to
		 * protected mode it is safe to assume that RPL 0 is a good
		 * default value.
		 */
		if (seg == VCPU_SREG_CS || seg == VCPU_SREG_SS)
			save->selector &= ~SEGMENT_RPL_MASK;
		save->dpl = save->selector & SEGMENT_RPL_MASK;
		save->s = 1;
	}
	vmx_set_segment(vcpu, save, seg);
}
