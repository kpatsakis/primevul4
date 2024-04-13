bool kvm_vcpu_is_reset_bsp(struct kvm_vcpu *vcpu)
{
	return vcpu->kvm->arch.bsp_vcpu_id == vcpu->vcpu_id;
}
