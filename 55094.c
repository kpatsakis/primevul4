void kvm_arch_destroy_vm(struct kvm *kvm)
{
	unsigned int i;
	struct kvm_vcpu *vcpu;

#ifdef CONFIG_KVM_XICS
	/*
	 * We call kick_all_cpus_sync() to ensure that all
	 * CPUs have executed any pending IPIs before we
	 * continue and free VCPUs structures below.
	 */
	if (is_kvmppc_hv_enabled(kvm))
		kick_all_cpus_sync();
#endif

	kvm_for_each_vcpu(i, vcpu, kvm)
		kvm_arch_vcpu_free(vcpu);

	mutex_lock(&kvm->lock);
	for (i = 0; i < atomic_read(&kvm->online_vcpus); i++)
		kvm->vcpus[i] = NULL;

	atomic_set(&kvm->online_vcpus, 0);

	kvmppc_core_destroy_vm(kvm);

	mutex_unlock(&kvm->lock);

	/* drop the module reference */
	module_put(kvm->arch.kvm_ops->owner);
}
