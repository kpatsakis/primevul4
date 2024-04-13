static void kvm_gen_update_masterclock(struct kvm *kvm)
{
#ifdef CONFIG_X86_64
	int i;
	struct kvm_vcpu *vcpu;
	struct kvm_arch *ka = &kvm->arch;

	spin_lock(&ka->pvclock_gtod_sync_lock);
	kvm_make_mclock_inprogress_request(kvm);
	/* no guest entries from this point */
	pvclock_update_vm_gtod_copy(kvm);

	kvm_for_each_vcpu(i, vcpu, kvm)
		kvm_make_request(KVM_REQ_CLOCK_UPDATE, vcpu);

	/* guest entries allowed */
	kvm_for_each_vcpu(i, vcpu, kvm)
		clear_bit(KVM_REQ_MCLOCK_INPROGRESS, &vcpu->requests);

	spin_unlock(&ka->pvclock_gtod_sync_lock);
#endif
}
