static void vmx_destroy_pml_buffer(struct vcpu_vmx *vmx)
{
	if (vmx->pml_pg) {
		__free_page(vmx->pml_pg);
		vmx->pml_pg = NULL;
	}
}
