void __audit_ipc_obj(struct kern_ipc_perm *ipcp)
{
	struct audit_context *context = current->audit_context;
	context->ipc.uid = ipcp->uid;
	context->ipc.gid = ipcp->gid;
	context->ipc.mode = ipcp->mode;
	context->ipc.has_perm = 0;
	security_ipc_getsecid(ipcp, &context->ipc.osid);
	context->type = AUDIT_IPC;
}
