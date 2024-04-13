fst_init(void)
{
	int i;

	for (i = 0; i < FST_MAX_CARDS; i++)
		fst_card_array[i] = NULL;
	spin_lock_init(&fst_work_q_lock);
	return pci_register_driver(&fst_driver);
}
