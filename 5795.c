  explicit Buffer(std::size_t capacity, std::size_t memory_limit)
      : capacity_(capacity), memory_limit_(memory_limit), current_bytes_(0) {}