void ipc_rmid(struct ipc_ids *ids, struct kern_ipc_perm *ipcp)
{
	int lid = ipcid_to_idx(ipcp->id);

	idr_remove(&ids->ipcs_idr, lid);
	ids->in_use--;
	ipcp->deleted = true;
}
