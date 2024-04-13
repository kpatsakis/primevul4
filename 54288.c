int mem_cgroup_sockets_init(struct cgroup *cgrp, struct cgroup_subsys *ss)
{
	struct proto *proto;
	int ret = 0;

	mutex_lock(&proto_list_mutex);
	list_for_each_entry(proto, &proto_list, node) {
		if (proto->init_cgroup) {
			ret = proto->init_cgroup(cgrp, ss);
			if (ret)
				goto out;
		}
	}

	mutex_unlock(&proto_list_mutex);
	return ret;
out:
	list_for_each_entry_continue_reverse(proto, &proto_list, node)
		if (proto->destroy_cgroup)
			proto->destroy_cgroup(cgrp);
	mutex_unlock(&proto_list_mutex);
	return ret;
}
