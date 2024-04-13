SIZED_STRING* yr_re_ast_extract_literal(
    RE_AST* re_ast)
{
  SIZED_STRING* string;
  RE_NODE* node = re_ast->root_node;

  int i, length = 0;
  char tmp;

  while (node != NULL)
  {
    length++;

    if (node->type == RE_NODE_LITERAL)
      break;

    if (node->type != RE_NODE_CONCAT)
      return NULL;

    if (node->right == NULL ||
        node->right->type != RE_NODE_LITERAL)
      return NULL;

    node = node->left;
  }

  string = (SIZED_STRING*) yr_malloc(sizeof(SIZED_STRING) + length);

  if (string == NULL)
    return NULL;

  string->length = 0;

  node = re_ast->root_node;

  while (node->type == RE_NODE_CONCAT)
  {
    string->c_string[string->length++] = node->right->value;
    node = node->left;
  }

  string->c_string[string->length++] = node->value;


  for (i = 0; i < length / 2; i++)
  {
    tmp = string->c_string[i];
    string->c_string[i] = string->c_string[length - i - 1];
    string->c_string[length - i - 1] = tmp;
  }

  return string;
}
