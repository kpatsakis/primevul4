static int __init ipc_init(void)
{
	sem_init();
	msg_init();
	shm_init();
	return 0;
}
