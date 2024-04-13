static void kvmclock_sync_fn(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct kvm_arch *ka = container_of(dwork, struct kvm_arch,
					   kvmclock_sync_work);
	struct kvm *kvm = container_of(ka, struct kvm, arch);

	schedule_delayed_work(&kvm->arch.kvmclock_update_work, 0);
	schedule_delayed_work(&kvm->arch.kvmclock_sync_work,
					KVMCLOCK_SYNC_PERIOD);
}
