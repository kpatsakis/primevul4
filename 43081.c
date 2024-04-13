static void pvclock_gtod_update_fn(struct work_struct *work)
{
	struct kvm *kvm;

	struct kvm_vcpu *vcpu;
	int i;

	spin_lock(&kvm_lock);
	list_for_each_entry(kvm, &vm_list, vm_list)
		kvm_for_each_vcpu(i, vcpu, kvm)
			kvm_make_request(KVM_REQ_MASTERCLOCK_UPDATE, vcpu);
	atomic_set(&kvm_guest_has_master_clock, 0);
	spin_unlock(&kvm_lock);
}
