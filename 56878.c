static inline void set_cookie(struct sk_buff *skb, char code)
{
	((char *)skb->data)[NOTIFY_COOKIE_LEN-1] = code;
}
