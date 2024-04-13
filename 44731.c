static bool is_ready_state(int status)
{
	int state = status & (STATUS_READY | STATUS_DIR | STATUS_DMA);
	return state == STATUS_READY;
}
