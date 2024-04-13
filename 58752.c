void __fsnotify_vfsmount_delete(struct vfsmount *mnt)
{
	fsnotify_clear_marks_by_mount(mnt);
}
