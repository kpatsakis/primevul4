static void tg3_link_report(struct tg3 *tp)
{
	if (!netif_carrier_ok(tp->dev)) {
		netif_info(tp, link, tp->dev, "Link is down\n");
		tg3_ump_link_report(tp);
	} else if (netif_msg_link(tp)) {
		netdev_info(tp->dev, "Link is up at %d Mbps, %s duplex\n",
			    (tp->link_config.active_speed == SPEED_1000 ?
			     1000 :
			     (tp->link_config.active_speed == SPEED_100 ?
			      100 : 10)),
			    (tp->link_config.active_duplex == DUPLEX_FULL ?
			     "full" : "half"));

		netdev_info(tp->dev, "Flow control is %s for TX and %s for RX\n",
			    (tp->link_config.active_flowctrl & FLOW_CTRL_TX) ?
			    "on" : "off",
			    (tp->link_config.active_flowctrl & FLOW_CTRL_RX) ?
			    "on" : "off");

		if (tp->phy_flags & TG3_PHYFLG_EEE_CAP)
			netdev_info(tp->dev, "EEE is %s\n",
				    tp->setlpicnt ? "enabled" : "disabled");

		tg3_ump_link_report(tp);
	}

	tp->link_up = netif_carrier_ok(tp->dev);
}
