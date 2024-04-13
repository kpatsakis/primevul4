void kvm_arch_destroy_vm(struct kvm *kvm)
{
	if (current->mm == kvm->mm) {
		/*
		 * Free memory regions allocated on behalf of userspace,
		 * unless the the memory map has changed due to process exit
		 * or fd copying.
		 */
		x86_set_memory_region(kvm, APIC_ACCESS_PAGE_PRIVATE_MEMSLOT, 0, 0);
		x86_set_memory_region(kvm, IDENTITY_PAGETABLE_PRIVATE_MEMSLOT, 0, 0);
		x86_set_memory_region(kvm, TSS_PRIVATE_MEMSLOT, 0, 0);
	}
	kvm_iommu_unmap_guest(kvm);
	kfree(kvm->arch.vpic);
	kfree(kvm->arch.vioapic);
	kvm_free_vcpus(kvm);
	kfree(rcu_dereference_check(kvm->arch.apic_map, 1));
}
