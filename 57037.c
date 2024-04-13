static void f2fs_build_fault_attr(struct f2fs_sb_info *sbi,
						unsigned int rate)
{
	struct f2fs_fault_info *ffi = &sbi->fault_info;

	if (rate) {
		atomic_set(&ffi->inject_ops, 0);
		ffi->inject_rate = rate;
		ffi->inject_type = (1 << FAULT_MAX) - 1;
	} else {
		memset(ffi, 0, sizeof(struct f2fs_fault_info));
	}
}
