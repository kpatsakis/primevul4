int snd_card_locked(int card)
{
	int locked;

	mutex_lock(&snd_card_mutex);
	locked = test_bit(card, snd_cards_lock);
	mutex_unlock(&snd_card_mutex);
	return locked;
}
