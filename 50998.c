static int ffs_func_revmap_intf(struct ffs_function *func, u8 intf)
{
	short *nums = func->interfaces_nums;
	unsigned count = func->ffs->interfaces_count;

	for (; count; --count, ++nums) {
		if (*nums >= 0 && *nums == intf)
			return nums - func->interfaces_nums;
	}

	return -EDOM;
}
