#include "communication/Comms.hpp"

#include <utility>

Comms::Comms(int port, MessageHandler messageHandler):
    port(port),
    messageHandler(std::move(messageHandler)) {}

int Comms::getPort() const {
    return port;
}

const MessageHandler &Comms::getMessageHandler() const {
    return messageHandler;
}
