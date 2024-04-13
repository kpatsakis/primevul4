void netdev_run_todo(void)
{
	struct list_head list;

	/* Snapshot list, allow later requests */
	list_replace_init(&net_todo_list, &list);

	__rtnl_unlock();

	while (!list_empty(&list)) {
		struct net_device *dev
			= list_first_entry(&list, struct net_device, todo_list);
		list_del(&dev->todo_list);

		if (unlikely(dev->reg_state != NETREG_UNREGISTERING)) {
			printk(KERN_ERR "network todo '%s' but state %d\n",
			       dev->name, dev->reg_state);
			dump_stack();
			continue;
		}

		dev->reg_state = NETREG_UNREGISTERED;

		on_each_cpu(flush_backlog, dev, 1);

		netdev_wait_allrefs(dev);

		/* paranoia */
		BUG_ON(netdev_refcnt_read(dev));
		WARN_ON(rcu_dereference_raw(dev->ip_ptr));
		WARN_ON(rcu_dereference_raw(dev->ip6_ptr));
		WARN_ON(dev->dn_ptr);

		if (dev->destructor)
			dev->destructor(dev);

		/* Free network device */
		kobject_put(&dev->dev.kobj);
	}
}
