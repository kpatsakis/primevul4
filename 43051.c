static void kvm_gen_kvmclock_update(struct kvm_vcpu *v)
{
	struct kvm *kvm = v->kvm;

	kvm_make_request(KVM_REQ_CLOCK_UPDATE, v);
	schedule_delayed_work(&kvm->arch.kvmclock_update_work,
					KVMCLOCK_UPDATE_DELAY);
}
