int kvm_arch_vcpu_runnable(struct kvm_vcpu *v)
{
	return !!(v->arch.pending_exceptions) || kvm_request_pending(v);
}
