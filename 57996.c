static int mailimf_zone_parse(const char * message, size_t length,
                              size_t * indx, int * result)
{
  int zone;
  int sign;
  size_t cur_token;
  int r;
  uint32_t value;

  cur_token = * indx;

  if (cur_token + 1 < length) {
    if ((message[cur_token] == 'U') && (message[cur_token + 1] == 'T')) {
      * result = TRUE;
      * indx = cur_token + 2;

      return MAILIMF_NO_ERROR;
    }
  }

  zone = 0;
  if (cur_token + 2 < length) {
    int state;

    state = STATE_ZONE_1;

    while (state <= 2) {
      switch (state) {
        case STATE_ZONE_1:
          switch (message[cur_token]) {
            case 'G':
              if (message[cur_token + 1] == 'M' && message[cur_token + 2] == 'T') {
                if ((cur_token + 3 < length) && ((message[cur_token + 3] == '+') || (message[cur_token + 3] == '-'))) {
                  cur_token += 3;
                  state = STATE_ZONE_CONT;
                }
                else {
                  zone = 0;
                  state = STATE_ZONE_OK;
                }
              }
              else {
                state = STATE_ZONE_ERR;
              }
              break;
            case 'E':
              zone = -5;
              state = STATE_ZONE_2;
              break;
            case 'C':
              zone = -6;
              state = STATE_ZONE_2;
              break;
            case 'M':
              zone = -7;
              state = STATE_ZONE_2;
              break;
            case 'P':
              zone = -8;
              state = STATE_ZONE_2;
              break;
            default:
              state = STATE_ZONE_CONT;
              break;
          }
          break;
        case STATE_ZONE_2:
          switch (message[cur_token + 1]) {
            case 'S':
              state = STATE_ZONE_3;
              break;
            case 'D':
              zone ++;
              state = STATE_ZONE_3;
              break;
            default:
              state = STATE_ZONE_ERR;
              break;
          }
          break;
        case STATE_ZONE_3:
          if (message[cur_token + 2] == 'T') {
            zone *= 100;
            state = STATE_ZONE_OK;
          }
          else
            state = STATE_ZONE_ERR;
          break;
      }
    }

    switch (state) {
      case STATE_ZONE_OK:
        * result = zone;
        * indx = cur_token + 3;
        return MAILIMF_NO_ERROR;

      case STATE_ZONE_ERR:
        return MAILIMF_ERROR_PARSE;
    }
  }

  sign = 1;
  r = mailimf_plus_parse(message, length, &cur_token);
  if (r == MAILIMF_NO_ERROR)
    sign = 1;

  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_minus_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      sign = -1;
  }

  if (r == MAILIMF_NO_ERROR) {
    /* do nothing */
  }
  else if (r == MAILIMF_ERROR_PARSE)
    sign = 1;
  else
    return r;

  r = mailimf_number_parse(message, length, &cur_token, &value);
  if (r != MAILIMF_NO_ERROR)
    return r;

  zone = value * sign;

  * indx = cur_token;
  * result = zone;

  return MAILIMF_NO_ERROR;
}
