int yr_re_ast_contains_dot_star(
    RE_AST* re_ast)
{
  return _yr_re_node_contains_dot_star(re_ast->root_node);
}
