int __init proc_sys_init(void)
{
	struct proc_dir_entry *proc_sys_root;

	proc_sys_root = proc_mkdir("sys", NULL);
	proc_sys_root->proc_iops = &proc_sys_dir_operations;
	proc_sys_root->proc_fops = &proc_sys_dir_file_operations;
	proc_sys_root->nlink = 0;

	return sysctl_init();
}
