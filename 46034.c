static void emulator_get_idt(struct desc_ptr *dt, struct kvm_vcpu *vcpu)
{
	kvm_x86_ops->get_idt(vcpu, dt);
}
