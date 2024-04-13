monitor_sync(struct monitor *pmonitor)
{
	if (options.compression) {
		/* The member allocation is not visible, so sync it */
		mm_share_sync(&pmonitor->m_zlib, &pmonitor->m_zback);
	}
}
