static int kvm_sync_page(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp,
			 struct list_head *invalid_list)
{
	return __kvm_sync_page(vcpu, sp, invalid_list, true);
}
