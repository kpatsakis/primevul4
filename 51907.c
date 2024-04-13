ppp_find_channel(struct ppp_net *pn, int unit)
{
	struct channel *pch;

	list_for_each_entry(pch, &pn->new_channels, list) {
		if (pch->file.index == unit) {
			list_move(&pch->list, &pn->all_channels);
			return pch;
		}
	}

	list_for_each_entry(pch, &pn->all_channels, list) {
		if (pch->file.index == unit)
			return pch;
	}

	return NULL;
}
