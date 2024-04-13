static void emulator_set_cached_descriptor(struct desc_struct *desc, int seg,
					   struct kvm_vcpu *vcpu)
{
	struct kvm_segment var;

	/* needed to preserve selector */
	kvm_get_segment(vcpu, &var, seg);

	var.base = get_desc_base(desc);
	var.limit = get_desc_limit(desc);
	if (desc->g)
		var.limit = (var.limit << 12) | 0xfff;
	var.type = desc->type;
	var.present = desc->p;
	var.dpl = desc->dpl;
	var.db = desc->d;
	var.s = desc->s;
	var.l = desc->l;
	var.g = desc->g;
	var.avl = desc->avl;
	var.present = desc->p;
	var.unusable = !var.present;
	var.padding = 0;

	kvm_set_segment(vcpu, &var, seg);
	return;
}
