YR_API void yr_object_print_data(
    YR_OBJECT* object,
    int indent,
    int print_identifier)
{
  YR_DICTIONARY_ITEMS* dict_items;
  YR_ARRAY_ITEMS* array_items;
  YR_STRUCTURE_MEMBER* member;

  char indent_spaces[32];
  int i;

  indent = yr_min(indent, sizeof(indent_spaces) - 1);

  memset(indent_spaces, '\t', indent);
  indent_spaces[indent] = '\0';

  if (print_identifier && object->type != OBJECT_TYPE_FUNCTION)
    printf("%s%s", indent_spaces, object->identifier);

  switch(object->type)
  {
    case OBJECT_TYPE_INTEGER:

      if (object->value.i != UNDEFINED)
        printf(" = %" PRIu64, object->value.i);
      else
        printf(" = UNDEFINED");

      break;

    case OBJECT_TYPE_STRING:

      if (object->value.ss != NULL)
      {
        size_t l;
        printf(" = \"");

        for (l = 0; l < object->value.ss->length; l++)
        {
          char c = object->value.ss->c_string[l];

          if (isprint((unsigned char) c))
            printf("%c", c);
          else
            printf("\\x%02x", (unsigned char) c);
        }

        printf("\"");
      }
      else
      {
        printf(" = UNDEFINED");
      }

      break;

    case OBJECT_TYPE_STRUCTURE:

      member = object_as_structure(object)->members;

      while (member != NULL)
      {
        if (member->object->type != OBJECT_TYPE_FUNCTION)
        {
          printf("\n");
          yr_object_print_data(member->object, indent + 1, 1);
        }
        member = member->next;
      }

      break;

    case OBJECT_TYPE_ARRAY:

      array_items = object_as_array(object)->items;

      if (array_items != NULL)
      {
        for (i = 0; i < array_items->count; i++)
        {
          if (array_items->objects[i] != NULL)
          {
            printf("\n%s\t[%d]", indent_spaces, i);
            yr_object_print_data(array_items->objects[i], indent + 1, 0);
          }
        }
      }

      break;

    case OBJECT_TYPE_DICTIONARY:

      dict_items = object_as_dictionary(object)->items;

      if (dict_items != NULL)
      {
        for (i = 0; i < dict_items->used; i++)
        {
          printf("\n%s\t%s", indent_spaces, dict_items->objects[i].key);
          yr_object_print_data(dict_items->objects[i].obj, indent + 1, 0);
        }
      }

      break;
  }
}
