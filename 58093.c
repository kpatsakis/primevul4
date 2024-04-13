static void notifier_del_vio(struct hvc_struct *hp, int data)
{
	hp->irq_requested = 0;
}
