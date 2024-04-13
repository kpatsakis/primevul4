static void __exit ib_cm_cleanup(void)
{
	struct cm_timewait_info *timewait_info, *tmp;

	spin_lock_irq(&cm.lock);
	list_for_each_entry(timewait_info, &cm.timewait_list, list)
		cancel_delayed_work(&timewait_info->work.work);
	spin_unlock_irq(&cm.lock);

	ib_unregister_client(&cm_client);
	destroy_workqueue(cm.wq);

	list_for_each_entry_safe(timewait_info, tmp, &cm.timewait_list, list) {
		list_del(&timewait_info->list);
		kfree(timewait_info);
	}

	class_unregister(&cm_class);
	idr_destroy(&cm.local_id_table);
}
