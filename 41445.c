static char *tg3_phy_string(struct tg3 *tp)
{
	switch (tp->phy_id & TG3_PHY_ID_MASK) {
	case TG3_PHY_ID_BCM5400:	return "5400";
	case TG3_PHY_ID_BCM5401:	return "5401";
	case TG3_PHY_ID_BCM5411:	return "5411";
	case TG3_PHY_ID_BCM5701:	return "5701";
	case TG3_PHY_ID_BCM5703:	return "5703";
	case TG3_PHY_ID_BCM5704:	return "5704";
	case TG3_PHY_ID_BCM5705:	return "5705";
	case TG3_PHY_ID_BCM5750:	return "5750";
	case TG3_PHY_ID_BCM5752:	return "5752";
	case TG3_PHY_ID_BCM5714:	return "5714";
	case TG3_PHY_ID_BCM5780:	return "5780";
	case TG3_PHY_ID_BCM5755:	return "5755";
	case TG3_PHY_ID_BCM5787:	return "5787";
	case TG3_PHY_ID_BCM5784:	return "5784";
	case TG3_PHY_ID_BCM5756:	return "5722/5756";
	case TG3_PHY_ID_BCM5906:	return "5906";
	case TG3_PHY_ID_BCM5761:	return "5761";
	case TG3_PHY_ID_BCM5718C:	return "5718C";
	case TG3_PHY_ID_BCM5718S:	return "5718S";
	case TG3_PHY_ID_BCM57765:	return "57765";
	case TG3_PHY_ID_BCM5719C:	return "5719C";
	case TG3_PHY_ID_BCM5720C:	return "5720C";
	case TG3_PHY_ID_BCM5762:	return "5762C";
	case TG3_PHY_ID_BCM8002:	return "8002/serdes";
	case 0:			return "serdes";
	default:		return "unknown";
	}
}
