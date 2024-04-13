static void emulator_set_segment_selector(u16 sel, int seg,
					  struct kvm_vcpu *vcpu)
{
	struct kvm_segment kvm_seg;

	kvm_get_segment(vcpu, &kvm_seg, seg);
	kvm_seg.selector = sel;
	kvm_set_segment(vcpu, &kvm_seg, seg);
}
