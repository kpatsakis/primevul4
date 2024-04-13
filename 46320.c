struct kern_ipc_perm *ipc_obtain_object_idr(struct ipc_ids *ids, int id)
{
	struct kern_ipc_perm *out;
	int lid = ipcid_to_idx(id);

	out = idr_find(&ids->ipcs_idr, lid);
	if (!out)
		return ERR_PTR(-EINVAL);

	return out;
}
