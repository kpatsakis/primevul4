static u16 tsk_adv_blocks(int len)
{
	return len / FLOWCTL_BLK_SZ / 4;
}
