int ib_cm_notify(struct ib_cm_id *cm_id, enum ib_event_type event)
{
	int ret;

	switch (event) {
	case IB_EVENT_COMM_EST:
		ret = cm_establish(cm_id);
		break;
	case IB_EVENT_PATH_MIG:
		ret = cm_migrate(cm_id);
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}
