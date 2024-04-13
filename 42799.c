static int harmonize_features(struct sk_buff *skb, __be16 protocol, int features)
{
	if (!can_checksum_protocol(features, protocol)) {
		features &= ~NETIF_F_ALL_CSUM;
		features &= ~NETIF_F_SG;
	} else if (illegal_highdma(skb->dev, skb)) {
		features &= ~NETIF_F_SG;
	}

	return features;
}
