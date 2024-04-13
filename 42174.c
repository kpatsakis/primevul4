static int hidp_queue_event(struct hidp_session *session, struct input_dev *dev,
				unsigned int type, unsigned int code, int value)
{
	unsigned char newleds;
	struct sk_buff *skb;

	BT_DBG("session %p type %d code %d value %d", session, type, code, value);

	if (type != EV_LED)
		return -1;

	newleds = (!!test_bit(LED_KANA,    dev->led) << 3) |
		  (!!test_bit(LED_COMPOSE, dev->led) << 3) |
		  (!!test_bit(LED_SCROLLL, dev->led) << 2) |
		  (!!test_bit(LED_CAPSL,   dev->led) << 1) |
		  (!!test_bit(LED_NUML,    dev->led));

	if (session->leds == newleds)
		return 0;

	session->leds = newleds;

	skb = alloc_skb(3, GFP_ATOMIC);
	if (!skb) {
		BT_ERR("Can't allocate memory for new frame");
		return -ENOMEM;
	}

	*skb_put(skb, 1) = HIDP_TRANS_DATA | HIDP_DATA_RTYPE_OUPUT;
	*skb_put(skb, 1) = 0x01;
	*skb_put(skb, 1) = newleds;

	skb_queue_tail(&session->intr_transmit, skb);

	hidp_schedule(session);

	return 0;
}
