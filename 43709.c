int kvm_init_shadow_ept_mmu(struct kvm_vcpu *vcpu, struct kvm_mmu *context,
		bool execonly)
{
	ASSERT(vcpu);
	ASSERT(!VALID_PAGE(vcpu->arch.mmu.root_hpa));

	context->shadow_root_level = kvm_x86_ops->get_tdp_level();

	context->nx = true;
	context->new_cr3 = paging_new_cr3;
	context->page_fault = ept_page_fault;
	context->gva_to_gpa = ept_gva_to_gpa;
	context->sync_page = ept_sync_page;
	context->invlpg = ept_invlpg;
	context->update_pte = ept_update_pte;
	context->free = paging_free;
	context->root_level = context->shadow_root_level;
	context->root_hpa = INVALID_PAGE;
	context->direct_map = false;

	update_permission_bitmask(vcpu, context, true);
	reset_rsvds_bits_mask_ept(vcpu, context, execonly);

	return 0;
}
