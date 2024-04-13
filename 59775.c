nfs_idmap_prepare_pipe_upcall(struct idmap *idmap,
		struct idmap_legacy_upcalldata *data)
{
	if (idmap->idmap_upcall_data != NULL) {
		WARN_ON_ONCE(1);
		return false;
	}
	idmap->idmap_upcall_data = data;
	return true;
}
