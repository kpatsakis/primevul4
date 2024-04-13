YR_OBJECT* _yr_object_lookup(
    YR_OBJECT* object,
    int flags,
    const char* pattern,
    va_list args)
{
  YR_OBJECT* obj = object;

  const char* p = pattern;
  const char* key = NULL;

  char str[256];

  int i;
  int index = -1;

  while (obj != NULL)
  {
    i = 0;

    while(*p != '\0' && *p != '.' && *p != '[' && i < sizeof(str) - 1)
    {
      str[i++] = *p++;
    }

    str[i] = '\0';

    if (obj->type != OBJECT_TYPE_STRUCTURE)
      return NULL;

    obj = yr_object_lookup_field(obj, str);

    if (obj == NULL)
      return NULL;

    if (*p == '[')
    {
      p++;

      if (*p == '%')
      {
        p++;

        switch(*p++)
        {
          case 'i':
            index = va_arg(args, int);
            break;
          case 's':
            key = va_arg(args, const char*);
            break;

          default:
            return NULL;
        }
      }
      else if (*p >= '0' && *p <= '9')
      {
        index = (int) strtol(p, (char**) &p, 10);
      }
      else if (*p == '"')
      {
        i = 0;
        p++;              // skip the opening quotation mark

        while (*p != '"' && *p != '\0' && i < sizeof(str))
          str[i++] = *p++;

        str[i] = '\0';
        p++;              // skip the closing quotation mark
        key = str;
      }
      else
      {
        return NULL;
      }

      assert(*p == ']');
      p++;
      assert(*p == '.' || *p == '\0');

      switch(obj->type)
      {
        case OBJECT_TYPE_ARRAY:
          assert(index != -1);
          obj = yr_object_array_get_item(obj, flags, index);
          break;

        case OBJECT_TYPE_DICTIONARY:
          assert(key != NULL);
          obj = yr_object_dict_get_item(obj, flags, key);
          break;
      }
    }

    if (*p == '\0')
      break;

    p++;
  }

  return obj;
}
