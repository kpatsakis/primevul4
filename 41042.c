keydirection2ascii (int kd, bool remote)
{
  if (kd == KEY_DIRECTION_BIDIRECTIONAL)
    return NULL;
  else if (kd == KEY_DIRECTION_NORMAL)
    return remote ? "1" : "0";
  else if (kd == KEY_DIRECTION_INVERSE)
    return remote ? "0" : "1";
  else
    {
      ASSERT (0);
    }
  return NULL; /* NOTREACHED */
}
