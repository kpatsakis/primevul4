static void vmx_vcpu_put(struct kvm_vcpu *vcpu)
{
	vmx_vcpu_pi_put(vcpu);

	__vmx_load_host_state(to_vmx(vcpu));
}
