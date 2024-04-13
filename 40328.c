static int do_fscreeninfo_to_user(struct fb_fix_screeninfo *fix,
				  struct fb_fix_screeninfo32 __user *fix32)
{
	__u32 data;
	int err;

	err = copy_to_user(&fix32->id, &fix->id, sizeof(fix32->id));

	data = (__u32) (unsigned long) fix->smem_start;
	err |= put_user(data, &fix32->smem_start);

	err |= put_user(fix->smem_len, &fix32->smem_len);
	err |= put_user(fix->type, &fix32->type);
	err |= put_user(fix->type_aux, &fix32->type_aux);
	err |= put_user(fix->visual, &fix32->visual);
	err |= put_user(fix->xpanstep, &fix32->xpanstep);
	err |= put_user(fix->ypanstep, &fix32->ypanstep);
	err |= put_user(fix->ywrapstep, &fix32->ywrapstep);
	err |= put_user(fix->line_length, &fix32->line_length);

	data = (__u32) (unsigned long) fix->mmio_start;
	err |= put_user(data, &fix32->mmio_start);

	err |= put_user(fix->mmio_len, &fix32->mmio_len);
	err |= put_user(fix->accel, &fix32->accel);
	err |= copy_to_user(fix32->reserved, fix->reserved,
			    sizeof(fix->reserved));

	return err;
}
