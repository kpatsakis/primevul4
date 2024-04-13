static int __init ib_cm_init(void)
{
	int ret;

	memset(&cm, 0, sizeof cm);
	INIT_LIST_HEAD(&cm.device_list);
	rwlock_init(&cm.device_lock);
	spin_lock_init(&cm.lock);
	cm.listen_service_table = RB_ROOT;
	cm.listen_service_id = be64_to_cpu(IB_CM_ASSIGN_SERVICE_ID);
	cm.remote_id_table = RB_ROOT;
	cm.remote_qp_table = RB_ROOT;
	cm.remote_sidr_table = RB_ROOT;
	idr_init(&cm.local_id_table);
	get_random_bytes(&cm.random_id_operand, sizeof cm.random_id_operand);
	INIT_LIST_HEAD(&cm.timewait_list);

	ret = class_register(&cm_class);
	if (ret) {
		ret = -ENOMEM;
		goto error1;
	}

	cm.wq = create_workqueue("ib_cm");
	if (!cm.wq) {
		ret = -ENOMEM;
		goto error2;
	}

	ret = ib_register_client(&cm_client);
	if (ret)
		goto error3;

	return 0;
error3:
	destroy_workqueue(cm.wq);
error2:
	class_unregister(&cm_class);
error1:
	idr_destroy(&cm.local_id_table);
	return ret;
}
