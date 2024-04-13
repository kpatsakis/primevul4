static void vmx_switch_vmcs(struct kvm_vcpu *vcpu, struct loaded_vmcs *vmcs)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int cpu;

	if (vmx->loaded_vmcs == vmcs)
		return;

	cpu = get_cpu();
	vmx->loaded_vmcs = vmcs;
	vmx_vcpu_put(vcpu);
	vmx_vcpu_load(vcpu, cpu);
	vcpu->cpu = cpu;
	put_cpu();
}
