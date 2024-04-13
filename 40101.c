void unregister_atmdevice_notifier(struct notifier_block *nb)
{
	atomic_notifier_chain_unregister(&atm_dev_notify_chain, nb);
}
