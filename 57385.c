R_API RConfigNode* r_config_set_i(RConfig *cfg, const char *name, const ut64 i) {
	char buf[128], *ov = NULL;
	if (!cfg || !name) {
		return NULL;
	}
	RConfigNode *node = r_config_node_get (cfg, name);
	if (node) {
		if (node->flags & CN_RO) {
			node = NULL;
			goto beach;
		}
		if (node->value) {
			ov = strdup (node->value);
			if (!ov) {
				node = NULL;
				goto beach;
			}
			free (node->value);
		}
		if (node->flags & CN_BOOL) {
			node->value = strdup (r_str_bool (i));
		} else {
			snprintf (buf, sizeof (buf) - 1, "%" PFMT64d, i);
			node->value = strdup (buf);
		}
		if (!node->value) {
			node = NULL;
			goto beach;
		}
		node->i_value = i;
	} else {
		if (!cfg->lock) {
			if (i < 1024) {
				snprintf (buf, sizeof (buf), "%" PFMT64d "", i);
			} else {
				snprintf (buf, sizeof (buf), "0x%08" PFMT64x "", i);
			}
			node = r_config_node_new (name, buf);
			if (!node) {
				node = NULL;
				goto beach;
			}
			node->flags = CN_RW | CN_OFFT;
			node->i_value = i;
			if (cfg->ht) {
				ht_insert (cfg->ht, node->name, node);
			}
			if (cfg->nodes) {
				r_list_append (cfg->nodes, node);
				cfg->n_nodes++;
			}
		} else {
			eprintf ("(locked: no new keys can be created (%s))\n", name);
		}
	}

	if (node && node->setter) {
		ut64 oi = node->i_value;
		int ret = node->setter (cfg->user, node);
		if (!ret) {
			node->i_value = oi;
			free (node->value);
			node->value = strdup (ov? ov: "");
		}
	}
beach:
	free (ov);
	return node;
}
