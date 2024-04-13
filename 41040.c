key_direction_state_init (struct key_direction_state *kds, int key_direction)
{
  CLEAR (*kds);
  switch (key_direction)
    {
    case KEY_DIRECTION_NORMAL:
      kds->out_key = 0;
      kds->in_key = 1;
      kds->need_keys = 2;
      break;
    case KEY_DIRECTION_INVERSE:
      kds->out_key = 1;
      kds->in_key = 0;
      kds->need_keys = 2;
      break;
    case KEY_DIRECTION_BIDIRECTIONAL:
      kds->out_key = 0;
      kds->in_key = 0;
      kds->need_keys = 1;
      break;
    default:
      ASSERT (0);
    }
}
