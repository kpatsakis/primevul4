nfs_idmap_abort_pipe_upcall(struct idmap *idmap, int ret)
{
	if (idmap->idmap_upcall_data != NULL)
		nfs_idmap_complete_pipe_upcall_locked(idmap, ret);
}
