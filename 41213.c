bool our_mnt(struct vfsmount *mnt)
{
	return check_mnt(real_mount(mnt));
}
