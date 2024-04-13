void yr_re_ast_destroy(
    RE_AST* re_ast)
{
  if (re_ast->root_node != NULL)
    yr_re_node_destroy(re_ast->root_node);

  yr_free(re_ast);
}
