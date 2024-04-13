static int kvm_read_guest_virt_system(gva_t addr, void *val, unsigned int bytes,
				      struct kvm_vcpu *vcpu,
				      struct x86_exception *exception)
{
	return kvm_read_guest_virt_helper(addr, val, bytes, vcpu, 0, exception);
}
