entry_guard_learned_bridge_identity(const tor_addr_port_t *addrport,
                                    const uint8_t *rsa_id_digest)
{
  guard_selection_t *gs = get_guard_selection_by_name("bridges",
                                                      GS_TYPE_BRIDGE,
                                                      0);
  if (!gs)
    return;

  entry_guard_t *g = get_sampled_guard_by_bridge_addr(gs, addrport);
  if (!g)
    return;

  int make_persistent = 0;

  if (tor_digest_is_zero(g->identity)) {
    memcpy(g->identity, rsa_id_digest, DIGEST_LEN);
    make_persistent = 1;
  } else if (tor_memeq(g->identity, rsa_id_digest, DIGEST_LEN)) {
    /* Nothing to see here; we learned something we already knew. */
    if (BUG(! g->is_persistent))
      make_persistent = 1;
  } else {
    char old_id[HEX_DIGEST_LEN+1];
    base16_encode(old_id, sizeof(old_id), g->identity, sizeof(g->identity));
    log_warn(LD_BUG, "We 'learned' an identity %s for a bridge at %s:%d, but "
             "we already knew a different one (%s). Ignoring the new info as "
             "possibly bogus.",
             hex_str((const char *)rsa_id_digest, DIGEST_LEN),
             fmt_and_decorate_addr(&addrport->addr), addrport->port,
             old_id);
    return; // redundant, but let's be clear: we're not making this persistent.
  }

  if (make_persistent) {
    g->is_persistent = 1;
    entry_guards_changed_for_guard_selection(gs);
  }
}
