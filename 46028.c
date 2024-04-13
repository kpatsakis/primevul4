static bool emulator_get_cached_descriptor(struct desc_struct *desc, int seg,
					   struct kvm_vcpu *vcpu)
{
	struct kvm_segment var;

	kvm_get_segment(vcpu, &var, seg);

	if (var.unusable)
		return false;

	if (var.g)
		var.limit >>= 12;
	set_desc_limit(desc, var.limit);
	set_desc_base(desc, (unsigned long)var.base);
	desc->type = var.type;
	desc->s = var.s;
	desc->dpl = var.dpl;
	desc->p = var.present;
	desc->avl = var.avl;
	desc->l = var.l;
	desc->d = var.db;
	desc->g = var.g;

	return true;
}
