static void shm_close(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct shm_file_data *sfd = shm_file_data(file);
	struct shmid_kernel *shp;
	struct ipc_namespace *ns = sfd->ns;

	down_write(&shm_ids(ns).rwsem);
	/* remove from the list of attaches of the shm segment */
	shp = shm_lock(ns, sfd->id);
	shp->shm_lprid = task_tgid_vnr(current);
	shp->shm_dtim = get_seconds();
	shp->shm_nattch--;
	if (shm_may_destroy(ns, shp))
		shm_destroy(ns, shp);
	else
		shm_unlock(shp);
	up_write(&shm_ids(ns).rwsem);
}
