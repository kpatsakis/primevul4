static void gs_update_state(struct gs_can *dev, struct can_frame *cf)
{
	struct can_device_stats *can_stats = &dev->can.can_stats;

	if (cf->can_id & CAN_ERR_RESTARTED) {
		dev->can.state = CAN_STATE_ERROR_ACTIVE;
		can_stats->restarts++;
	} else if (cf->can_id & CAN_ERR_BUSOFF) {
		dev->can.state = CAN_STATE_BUS_OFF;
		can_stats->bus_off++;
	} else if (cf->can_id & CAN_ERR_CRTL) {
		if ((cf->data[1] & CAN_ERR_CRTL_TX_WARNING) ||
		    (cf->data[1] & CAN_ERR_CRTL_RX_WARNING)) {
			dev->can.state = CAN_STATE_ERROR_WARNING;
			can_stats->error_warning++;
		} else if ((cf->data[1] & CAN_ERR_CRTL_TX_PASSIVE) ||
			   (cf->data[1] & CAN_ERR_CRTL_RX_PASSIVE)) {
			dev->can.state = CAN_STATE_ERROR_PASSIVE;
			can_stats->error_passive++;
		} else {
			dev->can.state = CAN_STATE_ERROR_ACTIVE;
		}
	}
}
