int copy_strings_kernel(int argc, const char *const *__argv,
			struct linux_binprm *bprm)
{
	int r;
	mm_segment_t oldfs = get_fs();
	struct user_arg_ptr argv = {
		.ptr.native = (const char __user *const  __user *)__argv,
	};

	set_fs(KERNEL_DS);
	r = copy_strings(argc, argv, bprm);
	set_fs(oldfs);

	return r;
}
