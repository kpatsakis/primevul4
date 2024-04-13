fst_process_rx_status(int rx_status, char *name)
{
	switch (rx_status) {
	case NET_RX_SUCCESS:
		{
			/*
			 * Nothing to do here
			 */
			break;
		}
	case NET_RX_DROP:
		{
			dbg(DBG_ASS, "%s: Received packet dropped\n", name);
			break;
		}
	}
}
