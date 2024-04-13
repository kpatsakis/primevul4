static void kvmclock_update_fn(struct work_struct *work)
{
	int i;
	struct delayed_work *dwork = to_delayed_work(work);
	struct kvm_arch *ka = container_of(dwork, struct kvm_arch,
					   kvmclock_update_work);
	struct kvm *kvm = container_of(ka, struct kvm, arch);
	struct kvm_vcpu *vcpu;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		kvm_make_request(KVM_REQ_CLOCK_UPDATE, vcpu);
		kvm_vcpu_kick(vcpu);
	}
}
