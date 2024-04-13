static bool mapping_level_dirty_bitmap(struct kvm_vcpu *vcpu, gfn_t large_gfn)
{
	return !gfn_to_memslot_dirty_bitmap(vcpu, large_gfn, true);
}
