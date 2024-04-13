static struct cm_work * cm_dequeue_work(struct cm_id_private *cm_id_priv)
{
	struct cm_work *work;

	if (list_empty(&cm_id_priv->work_list))
		return NULL;

	work = list_entry(cm_id_priv->work_list.next, struct cm_work, list);
	list_del(&work->list);
	return work;
}
