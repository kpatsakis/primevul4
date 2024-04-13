void mq_clear_sbinfo(struct ipc_namespace *ns)
{
	ns->mq_mnt->mnt_sb->s_fs_info = NULL;
}
