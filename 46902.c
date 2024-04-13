static struct pi_desc *vcpu_to_pi_desc(struct kvm_vcpu *vcpu)
{
	return &(to_vmx(vcpu)->pi_desc);
}
