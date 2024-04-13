static int guess_header_type(const char * message, size_t length, size_t indx)
{
  int state;
  int r;

  state = HEADER_START;
  
  while (1) {

    if (indx >= length)
      return MAILIMF_FIELD_NONE;

    switch(state) {
    case HEADER_START:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'B':
	return MAILIMF_FIELD_BCC;
      case 'C':
	state = HEADER_C;
	break;
      case 'D':
	return MAILIMF_FIELD_ORIG_DATE;
      case 'F':
	return MAILIMF_FIELD_FROM;
      case 'I':
	return MAILIMF_FIELD_IN_REPLY_TO;
      case 'K':
	return MAILIMF_FIELD_KEYWORDS;
      case 'M':
	return MAILIMF_FIELD_MESSAGE_ID;
      case 'R':
	state = HEADER_R;
	break;
      case 'T':
	return MAILIMF_FIELD_TO;
	break;
      case 'S':
	state = HEADER_S;
	break;
      default:
	return MAILIMF_FIELD_NONE;
      }
      break;
    case HEADER_C:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'O':
	return MAILIMF_FIELD_COMMENTS;
      case 'C':
	return MAILIMF_FIELD_CC;
      default:
	return MAILIMF_FIELD_NONE;
      }
      break;
    case HEADER_R:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'E':
	state = HEADER_RE;
	break;
      default:
	return MAILIMF_FIELD_NONE;
      }
      break;
    case HEADER_RE:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'F':
	return MAILIMF_FIELD_REFERENCES;
      case 'P':
	return MAILIMF_FIELD_REPLY_TO;
      case 'S':
        state = HEADER_RES;
        break;
      case 'T':
        return MAILIMF_FIELD_RETURN_PATH;
      default:
	return MAILIMF_FIELD_NONE;
      }
      break;
    case HEADER_S:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'E':
	return MAILIMF_FIELD_SENDER;
      case 'U':
	return MAILIMF_FIELD_SUBJECT;
      default:
	return MAILIMF_FIELD_NONE;
      }
      break;

    case HEADER_RES:
      r = mailimf_token_case_insensitive_parse(message,
          length, &indx, "ent-");
      if (r != MAILIMF_NO_ERROR)
        return MAILIMF_FIELD_NONE;
      
      if (indx >= length)
        return MAILIMF_FIELD_NONE;
      
      switch((char) toupper((unsigned char) message[indx])) {
      case 'D':
        return MAILIMF_FIELD_RESENT_DATE;
      case 'F':
        return MAILIMF_FIELD_RESENT_FROM;
      case 'S':
        return MAILIMF_FIELD_RESENT_SENDER;
      case 'T':
        return MAILIMF_FIELD_RESENT_TO;
      case 'C':
        return MAILIMF_FIELD_RESENT_CC;
      case 'B':
        return MAILIMF_FIELD_RESENT_BCC;
      case 'M':
        return MAILIMF_FIELD_RESENT_MSG_ID;
      default:
        return MAILIMF_FIELD_NONE;
      }
      break;
    }
    indx ++;
  }
}
