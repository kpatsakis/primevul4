static bool try_async_pf(struct kvm_vcpu *vcpu, bool prefault, gfn_t gfn,
			 gva_t gva, pfn_t *pfn, bool write, bool *writable)
{
	bool async;

	*pfn = gfn_to_pfn_async(vcpu->kvm, gfn, &async, write, writable);

	if (!async)
		return false; /* *pfn has correct page already */

	if (!prefault && can_do_async_pf(vcpu)) {
		trace_kvm_try_async_get_page(gva, gfn);
		if (kvm_find_async_pf_gfn(vcpu, gfn)) {
			trace_kvm_async_pf_doublefault(gva, gfn);
			kvm_make_request(KVM_REQ_APF_HALT, vcpu);
			return true;
		} else if (kvm_arch_setup_async_pf(vcpu, gva, gfn))
			return true;
	}

	*pfn = gfn_to_pfn_prot(vcpu->kvm, gfn, write, writable);

	return false;
}
