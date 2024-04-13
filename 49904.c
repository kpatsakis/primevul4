static int ap_bus_resume(struct device *dev)
{
	struct ap_device *ap_dev = to_ap_dev(dev);
	int rc;

	if (ap_suspend_flag) {
		ap_suspend_flag = 0;
		if (ap_interrupts_available()) {
			if (!ap_using_interrupts()) {
				rc = register_adapter_interrupt(&ap_airq);
				ap_airq_flag = (rc == 0);
			}
		} else {
			if (ap_using_interrupts()) {
				unregister_adapter_interrupt(&ap_airq);
				ap_airq_flag = 0;
			}
		}
		ap_query_configuration();
		if (!user_set_domain) {
			ap_domain_index = -1;
			ap_select_domain();
		}
		init_timer(&ap_config_timer);
		ap_config_timer.function = ap_config_timeout;
		ap_config_timer.data = 0;
		ap_config_timer.expires = jiffies + ap_config_time * HZ;
		add_timer(&ap_config_timer);
		ap_work_queue = create_singlethread_workqueue("kapwork");
		if (!ap_work_queue)
			return -ENOMEM;
		tasklet_enable(&ap_tasklet);
		if (!ap_using_interrupts())
			ap_schedule_poll_timer();
		else
			tasklet_schedule(&ap_tasklet);
		if (ap_thread_flag)
			rc = ap_poll_thread_start();
		else
			rc = 0;
	} else
		rc = 0;
	if (AP_QID_QUEUE(ap_dev->qid) != ap_domain_index) {
		spin_lock_bh(&ap_dev->lock);
		ap_dev->qid = AP_MKQID(AP_QID_DEVICE(ap_dev->qid),
				       ap_domain_index);
		spin_unlock_bh(&ap_dev->lock);
	}
	queue_work(ap_work_queue, &ap_config_work);

	return rc;
}
