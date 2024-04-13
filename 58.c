  const std::string& IPAddress() const {
    const Network* active = active_network();
    if (active != NULL)
      return active->ip_address();
    if (ethernet_)
      return ethernet_->ip_address();
    static std::string null_address("0.0.0.0");
    return null_address;
  }
