int kvmppc_handle_load(struct kvm_run *run, struct kvm_vcpu *vcpu,
		       unsigned int rt, unsigned int bytes,
		       int is_default_endian)
{
	return __kvmppc_handle_load(run, vcpu, rt, bytes, is_default_endian, 0);
}
