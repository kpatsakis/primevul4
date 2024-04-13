static void vhost_attach_cgroups_work(struct vhost_work *work)
{
	struct vhost_attach_cgroups_struct *s;

	s = container_of(work, struct vhost_attach_cgroups_struct, work);
	s->ret = cgroup_attach_task_all(s->owner, current);
}
