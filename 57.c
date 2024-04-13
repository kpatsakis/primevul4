  template<typename T> const T GetWirelessNetworkByPath(
      const std::vector<T>& networks, const std::string& path) const {
    typedef typename std::vector<T>::const_iterator iter_t;
    iter_t iter = std::find_if(networks.begin(), networks.end(),
                               WirelessNetwork::ServicePathEq(path));
    return (iter != networks.end()) ? *iter : NULL;
  }
