MOCK_IMPL(STATIC time_t,
randomize_time,(time_t now, time_t max_backdate))
{
  tor_assert(max_backdate > 0);

  time_t earliest = now - max_backdate;
  time_t latest = now;
  if (earliest <= 0)
    earliest = 1;
  if (latest <= earliest)
    latest = earliest + 1;

  return crypto_rand_time_range(earliest, latest);
}
