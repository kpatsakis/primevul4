static inline struct shmid_kernel *shm_lock(struct ipc_namespace *ns, int id)
{
	struct kern_ipc_perm *ipcp = ipc_lock(&shm_ids(ns), id);

	/*
	 * We raced in the idr lookup or with shm_destroy().  Either way, the
	 * ID is busted.
	 */
	WARN_ON(IS_ERR(ipcp));

	return container_of(ipcp, struct shmid_kernel, shm_perm);
}
