static bool handle_abnormal_pfn(struct kvm_vcpu *vcpu, gva_t gva, gfn_t gfn,
				pfn_t pfn, unsigned access, int *ret_val)
{
	bool ret = true;

	/* The pfn is invalid, report the error! */
	if (unlikely(is_error_pfn(pfn))) {
		*ret_val = kvm_handle_bad_page(vcpu, gfn, pfn);
		goto exit;
	}

	if (unlikely(is_noslot_pfn(pfn)))
		vcpu_cache_mmio_info(vcpu, gva, gfn, access);

	ret = false;
exit:
	return ret;
}
