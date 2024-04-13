static int get_invariant_sys_reg(u64 id, void __user *uaddr)
{
	struct sys_reg_params params;
	const struct sys_reg_desc *r;

	if (!index_to_params(id, &params))
		return -ENOENT;

	r = find_reg(&params, invariant_sys_regs, ARRAY_SIZE(invariant_sys_regs));
	if (!r)
		return -ENOENT;

	return reg_to_user(uaddr, &r->val, id);
}
