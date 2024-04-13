int kvm_write_guest_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			   void *data, unsigned long len)
{
	struct kvm_memslots *slots = kvm_memslots(kvm);
	int r;

	if (slots->generation != ghc->generation)
		kvm_gfn_to_hva_cache_init(kvm, ghc, ghc->gpa);

	if (kvm_is_error_hva(ghc->hva))
		return -EFAULT;

	r = copy_to_user((void __user *)ghc->hva, data, len);
	if (r)
		return -EFAULT;
	mark_page_dirty_in_slot(kvm, ghc->memslot, ghc->gpa >> PAGE_SHIFT);

	return 0;
}
