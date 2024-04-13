static int fb_get_fscreeninfo(struct fb_info *info, unsigned int cmd,
			      unsigned long arg)
{
	mm_segment_t old_fs;
	struct fb_fix_screeninfo fix;
	struct fb_fix_screeninfo32 __user *fix32;
	int err;

	fix32 = compat_ptr(arg);

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	err = do_fb_ioctl(info, cmd, (unsigned long) &fix);
	set_fs(old_fs);

	if (!err)
		err = do_fscreeninfo_to_user(&fix, fix32);

	return err;
}
