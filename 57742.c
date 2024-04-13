static inline size_t ColorTo565(const DDSVector3 point)
{
  size_t r = ClampToLimit(31.0f*point.x,31);
  size_t g = ClampToLimit(63.0f*point.y,63);
  size_t b = ClampToLimit(31.0f*point.z,31);

  return (r << 11) | (g << 5) | b;
}
