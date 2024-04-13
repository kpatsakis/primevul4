sdev_iter_snapshots(struct vnode *dvp, char *name)
{
	sdev_iter_datasets(dvp, ZFS_IOC_SNAPSHOT_LIST_NEXT, name);
}
