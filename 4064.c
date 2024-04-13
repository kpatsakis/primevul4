T cast_if_fits(size_t v) {
    size_t max = std::numeric_limits<T>::max();
    if (v > max) {
        throw std::runtime_error(format("Value too large, {:d} > {:d}", v, max));
    }
    return static_cast<T>(v);
}