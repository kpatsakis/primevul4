static uint32_t deserialize_gw_speed(uint8_t value) {
  uint32_t speed;
  uint32_t exp;

  if (!value) {
    return 0;
  }

  if (value == UINT8_MAX) {
    /* maximum value: also return maximum value */
    return MAX_SMARTGW_SPEED;
  }

  speed = (value >> 3) + 1;
  exp = value & 7;

  while (exp-- > 0) {
    speed *= 10;
  }
  return speed;
}
