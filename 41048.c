must_have_n_keys (const char *filename, const char *option, const struct key2 *key2, int n)
{
  if (key2->n < n)
    {
#ifdef ENABLE_SMALL
      msg (M_FATAL, "Key file '%s' used in --%s contains insufficient key material [keys found=%d required=%d]", filename, option, key2->n, n);
#else
      msg (M_FATAL, "Key file '%s' used in --%s contains insufficient key material [keys found=%d required=%d] -- try generating a new key file with '" PACKAGE " --genkey --secret [file]', or use the existing key file in bidirectional mode by specifying --%s without a key direction parameter", filename, option, key2->n, n, option);
#endif
    }
}
