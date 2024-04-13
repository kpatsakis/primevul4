static struct kvm_vcpu *vmx_create_vcpu(struct kvm *kvm, unsigned int id)
{
	int err;
	struct vcpu_vmx *vmx = kmem_cache_zalloc(kvm_vcpu_cache, GFP_KERNEL);
	int cpu;

	if (!vmx)
		return ERR_PTR(-ENOMEM);

	vmx->vpid = allocate_vpid();

	err = kvm_vcpu_init(&vmx->vcpu, kvm, id);
	if (err)
		goto free_vcpu;

	err = -ENOMEM;

	/*
	 * If PML is turned on, failure on enabling PML just results in failure
	 * of creating the vcpu, therefore we can simplify PML logic (by
	 * avoiding dealing with cases, such as enabling PML partially on vcpus
	 * for the guest, etc.
	 */
	if (enable_pml) {
		vmx->pml_pg = alloc_page(GFP_KERNEL | __GFP_ZERO);
		if (!vmx->pml_pg)
			goto uninit_vcpu;
	}

	vmx->guest_msrs = kmalloc(PAGE_SIZE, GFP_KERNEL);
	BUILD_BUG_ON(ARRAY_SIZE(vmx_msr_index) * sizeof(vmx->guest_msrs[0])
		     > PAGE_SIZE);

	if (!vmx->guest_msrs)
		goto free_pml;

	vmx->loaded_vmcs = &vmx->vmcs01;
	vmx->loaded_vmcs->vmcs = alloc_vmcs();
	vmx->loaded_vmcs->shadow_vmcs = NULL;
	if (!vmx->loaded_vmcs->vmcs)
		goto free_msrs;
	loaded_vmcs_init(vmx->loaded_vmcs);

	cpu = get_cpu();
	vmx_vcpu_load(&vmx->vcpu, cpu);
	vmx->vcpu.cpu = cpu;
	err = vmx_vcpu_setup(vmx);
	vmx_vcpu_put(&vmx->vcpu);
	put_cpu();
	if (err)
		goto free_vmcs;
	if (cpu_need_virtualize_apic_accesses(&vmx->vcpu)) {
		err = alloc_apic_access_page(kvm);
		if (err)
			goto free_vmcs;
	}

	if (enable_ept) {
		if (!kvm->arch.ept_identity_map_addr)
			kvm->arch.ept_identity_map_addr =
				VMX_EPT_IDENTITY_PAGETABLE_ADDR;
		err = init_rmode_identity_map(kvm);
		if (err)
			goto free_vmcs;
	}

	if (nested) {
		nested_vmx_setup_ctls_msrs(vmx);
		vmx->nested.vpid02 = allocate_vpid();
	}

	vmx->nested.posted_intr_nv = -1;
	vmx->nested.current_vmptr = -1ull;

	vmx->msr_ia32_feature_control_valid_bits = FEATURE_CONTROL_LOCKED;

	return &vmx->vcpu;

free_vmcs:
	free_vpid(vmx->nested.vpid02);
	free_loaded_vmcs(vmx->loaded_vmcs);
free_msrs:
	kfree(vmx->guest_msrs);
free_pml:
	vmx_destroy_pml_buffer(vmx);
uninit_vcpu:
	kvm_vcpu_uninit(&vmx->vcpu);
free_vcpu:
	free_vpid(vmx->vpid);
	kmem_cache_free(kvm_vcpu_cache, vmx);
	return ERR_PTR(err);
}
