int crypto_register_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&crypto_chain, nb);
}
