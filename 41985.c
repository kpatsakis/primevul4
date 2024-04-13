int __init shmem_init(void)
{
	int error;

	error = bdi_init(&shmem_backing_dev_info);
	if (error)
		goto out4;

	error = shmem_init_inodecache();
	if (error)
		goto out3;

	error = register_filesystem(&shmem_fs_type);
	if (error) {
		printk(KERN_ERR "Could not register tmpfs\n");
		goto out2;
	}

	shm_mnt = vfs_kern_mount(&shmem_fs_type, MS_NOUSER,
				 shmem_fs_type.name, NULL);
	if (IS_ERR(shm_mnt)) {
		error = PTR_ERR(shm_mnt);
		printk(KERN_ERR "Could not kern_mount tmpfs\n");
		goto out1;
	}
	return 0;

out1:
	unregister_filesystem(&shmem_fs_type);
out2:
	shmem_destroy_inodecache();
out3:
	bdi_destroy(&shmem_backing_dev_info);
out4:
	shm_mnt = ERR_PTR(error);
	return error;
}
