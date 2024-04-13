check_replay_iv_consistency (const struct key_type *kt, bool packet_id, bool use_iv)
{
  if (cfb_ofb_mode (kt) && !(packet_id && use_iv))
    msg (M_FATAL, "--no-replay or --no-iv cannot be used with a CFB or OFB mode cipher");
}
