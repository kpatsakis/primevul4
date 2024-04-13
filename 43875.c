static void nested_free_all_saved_vmcss(struct vcpu_vmx *vmx)
{
	struct vmcs02_list *item, *n;
	list_for_each_entry_safe(item, n, &vmx->nested.vmcs02_pool, list) {
		if (vmx->loaded_vmcs != &item->vmcs02)
			free_loaded_vmcs(&item->vmcs02);
		list_del(&item->list);
		kfree(item);
	}
	vmx->nested.vmcs02_num = 0;

	if (vmx->loaded_vmcs != &vmx->vmcs01)
		free_loaded_vmcs(&vmx->vmcs01);
}
