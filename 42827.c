static void rollback_registered_many(struct list_head *head)
{
	struct net_device *dev, *tmp;

	BUG_ON(dev_boot_phase);
	ASSERT_RTNL();

	list_for_each_entry_safe(dev, tmp, head, unreg_list) {
		/* Some devices call without registering
		 * for initialization unwind. Remove those
		 * devices and proceed with the remaining.
		 */
		if (dev->reg_state == NETREG_UNINITIALIZED) {
			pr_debug("unregister_netdevice: device %s/%p never "
				 "was registered\n", dev->name, dev);

			WARN_ON(1);
			list_del(&dev->unreg_list);
			continue;
		}

		BUG_ON(dev->reg_state != NETREG_REGISTERED);
	}

	/* If device is running, close it first. */
	dev_close_many(head);

	list_for_each_entry(dev, head, unreg_list) {
		/* And unlink it from device chain. */
		unlist_netdevice(dev);

		dev->reg_state = NETREG_UNREGISTERING;
	}

	synchronize_net();

	list_for_each_entry(dev, head, unreg_list) {
		/* Shutdown queueing discipline. */
		dev_shutdown(dev);


		/* Notify protocols, that we are about to destroy
		   this device. They should clean all the things.
		*/
		call_netdevice_notifiers(NETDEV_UNREGISTER, dev);

		if (!dev->rtnl_link_ops ||
		    dev->rtnl_link_state == RTNL_LINK_INITIALIZED)
			rtmsg_ifinfo(RTM_DELLINK, dev, ~0U);

		/*
		 *	Flush the unicast and multicast chains
		 */
		dev_uc_flush(dev);
		dev_mc_flush(dev);

		if (dev->netdev_ops->ndo_uninit)
			dev->netdev_ops->ndo_uninit(dev);

		/* Notifier chain MUST detach us from master device. */
		WARN_ON(dev->master);

		/* Remove entries from kobject tree */
		netdev_unregister_kobject(dev);
	}

	/* Process any work delayed until the end of the batch */
	dev = list_first_entry(head, struct net_device, unreg_list);
	call_netdevice_notifiers(NETDEV_UNREGISTER_BATCH, dev);

	rcu_barrier();

	list_for_each_entry(dev, head, unreg_list)
		dev_put(dev);
}
