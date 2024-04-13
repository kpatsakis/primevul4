static int atl2_get_regs_len(struct net_device *netdev)
{
#define ATL2_REGS_LEN 42
	return sizeof(u32) * ATL2_REGS_LEN;
}
