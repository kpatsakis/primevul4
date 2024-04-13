static int paging64_init_context_common(struct kvm_vcpu *vcpu,
					struct kvm_mmu *context,
					int level)
{
	context->nx = is_nx(vcpu);
	context->root_level = level;

	reset_rsvds_bits_mask(vcpu, context);
	update_permission_bitmask(vcpu, context, false);
	update_last_pte_bitmap(vcpu, context);

	ASSERT(is_pae(vcpu));
	context->new_cr3 = paging_new_cr3;
	context->page_fault = paging64_page_fault;
	context->gva_to_gpa = paging64_gva_to_gpa;
	context->sync_page = paging64_sync_page;
	context->invlpg = paging64_invlpg;
	context->update_pte = paging64_update_pte;
	context->free = paging_free;
	context->shadow_root_level = level;
	context->root_hpa = INVALID_PAGE;
	context->direct_map = false;
	return 0;
}
