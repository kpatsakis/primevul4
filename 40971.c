static int perf_copy_attr(struct perf_event_attr __user *uattr,
			  struct perf_event_attr *attr)
{
	u32 size;
	int ret;

	if (!access_ok(VERIFY_WRITE, uattr, PERF_ATTR_SIZE_VER0))
		return -EFAULT;

	/*
	 * zero the full structure, so that a short copy will be nice.
	 */
	memset(attr, 0, sizeof(*attr));

	ret = get_user(size, &uattr->size);
	if (ret)
		return ret;

	if (size > PAGE_SIZE)	/* silly large */
		goto err_size;

	if (!size)		/* abi compat */
		size = PERF_ATTR_SIZE_VER0;

	if (size < PERF_ATTR_SIZE_VER0)
		goto err_size;

	/*
	 * If we're handed a bigger struct than we know of,
	 * ensure all the unknown bits are 0 - i.e. new
	 * user-space does not rely on any kernel feature
	 * extensions we dont know about yet.
	 */
	if (size > sizeof(*attr)) {
		unsigned char __user *addr;
		unsigned char __user *end;
		unsigned char val;

		addr = (void __user *)uattr + sizeof(*attr);
		end  = (void __user *)uattr + size;

		for (; addr < end; addr++) {
			ret = get_user(val, addr);
			if (ret)
				return ret;
			if (val)
				goto err_size;
		}
		size = sizeof(*attr);
	}

	ret = copy_from_user(attr, uattr, size);
	if (ret)
		return -EFAULT;

	if (attr->__reserved_1)
		return -EINVAL;

	if (attr->sample_type & ~(PERF_SAMPLE_MAX-1))
		return -EINVAL;

	if (attr->read_format & ~(PERF_FORMAT_MAX-1))
		return -EINVAL;

	if (attr->sample_type & PERF_SAMPLE_BRANCH_STACK) {
		u64 mask = attr->branch_sample_type;

		/* only using defined bits */
		if (mask & ~(PERF_SAMPLE_BRANCH_MAX-1))
			return -EINVAL;

		/* at least one branch bit must be set */
		if (!(mask & ~PERF_SAMPLE_BRANCH_PLM_ALL))
			return -EINVAL;

		/* kernel level capture: check permissions */
		if ((mask & PERF_SAMPLE_BRANCH_PERM_PLM)
		    && perf_paranoid_kernel() && !capable(CAP_SYS_ADMIN))
			return -EACCES;

		/* propagate priv level, when not set for branch */
		if (!(mask & PERF_SAMPLE_BRANCH_PLM_ALL)) {

			/* exclude_kernel checked on syscall entry */
			if (!attr->exclude_kernel)
				mask |= PERF_SAMPLE_BRANCH_KERNEL;

			if (!attr->exclude_user)
				mask |= PERF_SAMPLE_BRANCH_USER;

			if (!attr->exclude_hv)
				mask |= PERF_SAMPLE_BRANCH_HV;
			/*
			 * adjust user setting (for HW filter setup)
			 */
			attr->branch_sample_type = mask;
		}
	}

	if (attr->sample_type & PERF_SAMPLE_REGS_USER) {
		ret = perf_reg_validate(attr->sample_regs_user);
		if (ret)
			return ret;
	}

	if (attr->sample_type & PERF_SAMPLE_STACK_USER) {
		if (!arch_perf_have_user_stack_dump())
			return -ENOSYS;

		/*
		 * We have __u32 type for the size, but so far
		 * we can only use __u16 as maximum due to the
		 * __u16 sample size limit.
		 */
		if (attr->sample_stack_user >= USHRT_MAX)
			ret = -EINVAL;
		else if (!IS_ALIGNED(attr->sample_stack_user, sizeof(u64)))
			ret = -EINVAL;
	}

out:
	return ret;

err_size:
	put_user(sizeof(*attr), &uattr->size);
	ret = -E2BIG;
	goto out;
}
