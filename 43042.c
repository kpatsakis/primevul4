void kvm_arch_sync_events(struct kvm *kvm)
{
	cancel_delayed_work_sync(&kvm->arch.kvmclock_sync_work);
	cancel_delayed_work_sync(&kvm->arch.kvmclock_update_work);
	kvm_free_all_assigned_devices(kvm);
	kvm_free_pit(kvm);
}
