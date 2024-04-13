static int kvm_fetch_guest_virt(gva_t addr, void *val, unsigned int bytes,
				struct kvm_vcpu *vcpu,
				struct x86_exception *exception)
{
	u32 access = (kvm_x86_ops->get_cpl(vcpu) == 3) ? PFERR_USER_MASK : 0;
	return kvm_read_guest_virt_helper(addr, val, bytes, vcpu,
					  access | PFERR_FETCH_MASK,
					  exception);
}
