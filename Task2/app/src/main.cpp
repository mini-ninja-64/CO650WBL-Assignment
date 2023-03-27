#include <iostream>

#include "protocol/v1/PacketV1.hpp"
#include "communication/Server.hpp"
#include "communication/Client.hpp"

#define SERVER_PORT 3000

#ifdef SERVER
static const uint16_t PACKET_ID = 0x0000;
#else
static const uint16_t PACKET_ID = 0xBEEF;
#endif

#ifndef TESTING
int main() {
    std::atomic<bool> appRunning = true;

    MessageHandler messageHandler = [&appRunning](const auto& packet) mutable {
        if(packet->getVersion() == 1) {
            auto packetHeader = dynamic_cast<PacketHeaderV1 *>(packet->getPacketHeader().get());

            auto packetBody = dynamic_cast<PacketBodyV1 *>(packet->getPacketBody().get());
            switch (packetBody->getBodyType()) {
                case PacketBodyV1::ASCII:
                    std::cout << "[" << packetHeader->getSenderId() << "] "
                              << dynamic_cast<AsciiPacketBodyV1 *>(packetBody)->asString()
                              << std::endl;
                    break;
                case PacketBodyV1::QUIT:
                    std::cout << "Received quit request, will shutdown application" << std::endl;
                    appRunning = false;
                    break;
            }
        }
    };
#ifdef SERVER
    std::unique_ptr<Comms> comms = std::make_unique<Server>(SERVER_PORT, messageHandler);
#else
    std::unique_ptr<Comms> comms = std::make_unique<Client>("127.0.0.1", SERVER_PORT, messageHandler);
#endif

    comms->start();


    while (appRunning) {
        std::string userInput;
        std::getline(std::cin, userInput);

        if(!comms->isRunning()) {
            appRunning = false;
            continue;
        }

        auto packetHeader = std::make_unique<PacketHeaderV1>(PACKET_ID);
        std::unique_ptr<PacketBodyV1> packetBody = nullptr;
        if(userInput == "QUIT") {
            std::cout << "Sending a QUIT packet to suspend the application" << std::endl;
            packetBody = std::make_unique<QuitPacketBodyV1>();
            appRunning = false;
        } else {
            packetBody = std::make_unique<AsciiPacketBodyV1>(userInput);
        }

        auto packet = std::make_unique<PacketV1>(std::move(packetHeader), std::move(packetBody));
        comms->sendPacket(std::move(packet));
    }

    comms->stop();
}
#endif
