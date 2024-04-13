static u16 emulator_get_segment_selector(int seg, struct kvm_vcpu *vcpu)
{
	struct kvm_segment kvm_seg;

	kvm_get_segment(vcpu, &kvm_seg, seg);
	return kvm_seg.selector;
}
