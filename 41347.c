static void tg3_get_estats(struct tg3 *tp, struct tg3_ethtool_stats *estats)
{
	struct tg3_ethtool_stats *old_estats = &tp->estats_prev;
	struct tg3_hw_stats *hw_stats = tp->hw_stats;

	ESTAT_ADD(rx_octets);
	ESTAT_ADD(rx_fragments);
	ESTAT_ADD(rx_ucast_packets);
	ESTAT_ADD(rx_mcast_packets);
	ESTAT_ADD(rx_bcast_packets);
	ESTAT_ADD(rx_fcs_errors);
	ESTAT_ADD(rx_align_errors);
	ESTAT_ADD(rx_xon_pause_rcvd);
	ESTAT_ADD(rx_xoff_pause_rcvd);
	ESTAT_ADD(rx_mac_ctrl_rcvd);
	ESTAT_ADD(rx_xoff_entered);
	ESTAT_ADD(rx_frame_too_long_errors);
	ESTAT_ADD(rx_jabbers);
	ESTAT_ADD(rx_undersize_packets);
	ESTAT_ADD(rx_in_length_errors);
	ESTAT_ADD(rx_out_length_errors);
	ESTAT_ADD(rx_64_or_less_octet_packets);
	ESTAT_ADD(rx_65_to_127_octet_packets);
	ESTAT_ADD(rx_128_to_255_octet_packets);
	ESTAT_ADD(rx_256_to_511_octet_packets);
	ESTAT_ADD(rx_512_to_1023_octet_packets);
	ESTAT_ADD(rx_1024_to_1522_octet_packets);
	ESTAT_ADD(rx_1523_to_2047_octet_packets);
	ESTAT_ADD(rx_2048_to_4095_octet_packets);
	ESTAT_ADD(rx_4096_to_8191_octet_packets);
	ESTAT_ADD(rx_8192_to_9022_octet_packets);

	ESTAT_ADD(tx_octets);
	ESTAT_ADD(tx_collisions);
	ESTAT_ADD(tx_xon_sent);
	ESTAT_ADD(tx_xoff_sent);
	ESTAT_ADD(tx_flow_control);
	ESTAT_ADD(tx_mac_errors);
	ESTAT_ADD(tx_single_collisions);
	ESTAT_ADD(tx_mult_collisions);
	ESTAT_ADD(tx_deferred);
	ESTAT_ADD(tx_excessive_collisions);
	ESTAT_ADD(tx_late_collisions);
	ESTAT_ADD(tx_collide_2times);
	ESTAT_ADD(tx_collide_3times);
	ESTAT_ADD(tx_collide_4times);
	ESTAT_ADD(tx_collide_5times);
	ESTAT_ADD(tx_collide_6times);
	ESTAT_ADD(tx_collide_7times);
	ESTAT_ADD(tx_collide_8times);
	ESTAT_ADD(tx_collide_9times);
	ESTAT_ADD(tx_collide_10times);
	ESTAT_ADD(tx_collide_11times);
	ESTAT_ADD(tx_collide_12times);
	ESTAT_ADD(tx_collide_13times);
	ESTAT_ADD(tx_collide_14times);
	ESTAT_ADD(tx_collide_15times);
	ESTAT_ADD(tx_ucast_packets);
	ESTAT_ADD(tx_mcast_packets);
	ESTAT_ADD(tx_bcast_packets);
	ESTAT_ADD(tx_carrier_sense_errors);
	ESTAT_ADD(tx_discards);
	ESTAT_ADD(tx_errors);

	ESTAT_ADD(dma_writeq_full);
	ESTAT_ADD(dma_write_prioq_full);
	ESTAT_ADD(rxbds_empty);
	ESTAT_ADD(rx_discards);
	ESTAT_ADD(rx_errors);
	ESTAT_ADD(rx_threshold_hit);

	ESTAT_ADD(dma_readq_full);
	ESTAT_ADD(dma_read_prioq_full);
	ESTAT_ADD(tx_comp_queue_full);

	ESTAT_ADD(ring_set_send_prod_index);
	ESTAT_ADD(ring_status_update);
	ESTAT_ADD(nic_irqs);
	ESTAT_ADD(nic_avoided_irqs);
	ESTAT_ADD(nic_tx_threshold_hit);

	ESTAT_ADD(mbuf_lwm_thresh_hit);
}
