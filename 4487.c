  folly::Optional<ClientTransportParameters> getClientTransportParams()
      override {
    std::vector<TransportParameter> transportParams;
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::initial_max_stream_data_bidi_local,
        kDefaultStreamWindowSize));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::initial_max_stream_data_bidi_remote,
        kDefaultStreamWindowSize));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::initial_max_stream_data_uni,
        kDefaultStreamWindowSize));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::initial_max_streams_bidi,
        kDefaultMaxStreamsBidirectional));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::initial_max_streams_uni,
        kDefaultMaxStreamsUnidirectional));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::initial_max_data, kDefaultConnectionWindowSize));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::idle_timeout, kDefaultIdleTimeout.count()));
    transportParams.push_back(encodeIntegerParameter(
        TransportParameterId::max_packet_size, maxRecvPacketSize));
    if (clientActiveConnectionIdLimit_) {
      transportParams.push_back(encodeIntegerParameter(
          TransportParameterId::active_connection_id_limit,
          *clientActiveConnectionIdLimit_));
    }
    transportParams.push_back(encodeConnIdParameter(
        TransportParameterId::initial_source_connection_id,
        getTestConnectionId()));

    return ClientTransportParameters{std::move(transportParams)};
  }