void realmode_lidt(struct kvm_vcpu *vcpu, u16 limit, unsigned long base)
{
	struct desc_ptr dt = { limit, base };

	kvm_x86_ops->set_idt(vcpu, &dt);
}
