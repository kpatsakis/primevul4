static int enter_vmx_operation(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs *shadow_vmcs;

	if (cpu_has_vmx_msr_bitmap()) {
		vmx->nested.msr_bitmap =
				(unsigned long *)__get_free_page(GFP_KERNEL);
		if (!vmx->nested.msr_bitmap)
			goto out_msr_bitmap;
	}

	vmx->nested.cached_vmcs12 = kmalloc(VMCS12_SIZE, GFP_KERNEL);
	if (!vmx->nested.cached_vmcs12)
		goto out_cached_vmcs12;

	if (enable_shadow_vmcs) {
		shadow_vmcs = alloc_vmcs();
		if (!shadow_vmcs)
			goto out_shadow_vmcs;
		/* mark vmcs as shadow */
		shadow_vmcs->revision_id |= (1u << 31);
		/* init shadow vmcs */
		vmcs_clear(shadow_vmcs);
		vmx->vmcs01.shadow_vmcs = shadow_vmcs;
	}

	INIT_LIST_HEAD(&(vmx->nested.vmcs02_pool));
	vmx->nested.vmcs02_num = 0;

	hrtimer_init(&vmx->nested.preemption_timer, CLOCK_MONOTONIC,
		     HRTIMER_MODE_REL_PINNED);
	vmx->nested.preemption_timer.function = vmx_preemption_timer_fn;

	vmx->nested.vmxon = true;
	return 0;

out_shadow_vmcs:
	kfree(vmx->nested.cached_vmcs12);

out_cached_vmcs12:
	free_page((unsigned long)vmx->nested.msr_bitmap);

out_msr_bitmap:
	return -ENOMEM;
}
