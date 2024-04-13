static inline void VectorMultiplyAdd(const DDSVector4 a, const DDSVector4 b,
  const DDSVector4 c, DDSVector4 *destination)
{
  destination->x = (a.x * b.x) + c.x;
  destination->y = (a.y * b.y) + c.y;
  destination->z = (a.z * b.z) + c.z;
  destination->w = (a.w * b.w) + c.w;
}
