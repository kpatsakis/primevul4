void kvm_arch_destroy_vm(struct kvm *kvm)
{
	if (current->mm == kvm->mm) {
		/*
		 * Free memory regions allocated on behalf of userspace,
		 * unless the the memory map has changed due to process exit
		 * or fd copying.
		 */
		struct kvm_userspace_memory_region mem;
		memset(&mem, 0, sizeof(mem));
		mem.slot = APIC_ACCESS_PAGE_PRIVATE_MEMSLOT;
		kvm_set_memory_region(kvm, &mem);

		mem.slot = IDENTITY_PAGETABLE_PRIVATE_MEMSLOT;
		kvm_set_memory_region(kvm, &mem);

		mem.slot = TSS_PRIVATE_MEMSLOT;
		kvm_set_memory_region(kvm, &mem);
	}
	kvm_iommu_unmap_guest(kvm);
	kfree(kvm->arch.vpic);
	kfree(kvm->arch.vioapic);
	kvm_free_vcpus(kvm);
	if (kvm->arch.apic_access_page)
		put_page(kvm->arch.apic_access_page);
	kfree(rcu_dereference_check(kvm->arch.apic_map, 1));
}
