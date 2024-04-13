nfs_idmap_complete_pipe_upcall_locked(struct idmap *idmap, int ret)
{
	struct key_construction *cons = idmap->idmap_upcall_data->key_cons;

	kfree(idmap->idmap_upcall_data);
	idmap->idmap_upcall_data = NULL;
	complete_request_key(cons, ret);
}
