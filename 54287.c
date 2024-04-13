void mem_cgroup_sockets_destroy(struct cgroup *cgrp)
{
	struct proto *proto;

	mutex_lock(&proto_list_mutex);
	list_for_each_entry_reverse(proto, &proto_list, node)
		if (proto->destroy_cgroup)
			proto->destroy_cgroup(cgrp);
	mutex_unlock(&proto_list_mutex);
}
