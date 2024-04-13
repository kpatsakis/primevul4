ipip6_tunnel_add_prl(struct ip_tunnel *t, struct ip_tunnel_prl *a, int chg)
{
	struct ip_tunnel_prl_entry *p;
	int err = 0;

	if (a->addr == htonl(INADDR_ANY))
		return -EINVAL;

	ASSERT_RTNL();

	for (p = rtnl_dereference(t->prl); p; p = rtnl_dereference(p->next)) {
		if (p->addr == a->addr) {
			if (chg) {
				p->flags = a->flags;
				goto out;
			}
			err = -EEXIST;
			goto out;
		}
	}

	if (chg) {
		err = -ENXIO;
		goto out;
	}

	p = kzalloc(sizeof(struct ip_tunnel_prl_entry), GFP_KERNEL);
	if (!p) {
		err = -ENOBUFS;
		goto out;
	}

	p->next = t->prl;
	p->addr = a->addr;
	p->flags = a->flags;
	t->prl_count++;
	rcu_assign_pointer(t->prl, p);
out:
	return err;
}
