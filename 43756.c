static int mapping_level(struct kvm_vcpu *vcpu, gfn_t large_gfn)
{
	int host_level, level, max_level;

	host_level = host_mapping_level(vcpu->kvm, large_gfn);

	if (host_level == PT_PAGE_TABLE_LEVEL)
		return host_level;

	max_level = min(kvm_x86_ops->get_lpage_level(), host_level);

	for (level = PT_DIRECTORY_LEVEL; level <= max_level; ++level)
		if (has_wrprotected_page(vcpu->kvm, large_gfn, level))
			break;

	return level - 1;
}
