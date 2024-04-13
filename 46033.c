static void emulator_get_gdt(struct desc_ptr *dt, struct kvm_vcpu *vcpu)
{
	kvm_x86_ops->get_gdt(vcpu, dt);
}
