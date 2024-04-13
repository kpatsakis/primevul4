static void edge_heartbeat_schedule(struct edgeport_serial *edge_serial)
{
	if (!edge_serial->use_heartbeat)
		return;

	schedule_delayed_work(&edge_serial->heartbeat_work,
			FW_HEARTBEAT_SECS * HZ);
}
