static int vmx_create_pml_buffer(struct vcpu_vmx *vmx)
{
	struct page *pml_pg;

	pml_pg = alloc_page(GFP_KERNEL | __GFP_ZERO);
	if (!pml_pg)
		return -ENOMEM;

	vmx->pml_pg = pml_pg;

	vmcs_write64(PML_ADDRESS, page_to_phys(vmx->pml_pg));
	vmcs_write16(GUEST_PML_INDEX, PML_ENTITY_NUM - 1);

	return 0;
}
