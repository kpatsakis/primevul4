static inline size_t ClampToLimit(const float value,
  const size_t limit)
{
  size_t
    result = (int) (value + 0.5f);

  if (result < 0.0f)
    return(0);
  if (result > limit)
    return(limit);
  return result;
}
