#include <iostream>
#include <chrono>

#include "protocol/v1/PacketV1.hpp"
#include "communication/Server.hpp"
#include "communication/Client.hpp"

#ifndef TESTING

#define SERVER_PORT 3000

#ifdef SERVER
static const uint16_t SENDER_ID = 0x0000;
#else
static const uint16_t SENDER_ID = 0x0001;
#endif

static void inputHandlerThreadFunction(std::atomic<bool>& inputHandlerRunning, const std::unique_ptr<Comms>& comms) {
    while (inputHandlerRunning) {
        std::string userInput;
        std::getline(std::cin, userInput);

        if(!comms->isRunning()) {
            inputHandlerRunning = false;
            continue;
        }

        auto packetHeader = std::make_unique<PacketHeaderV1>(SENDER_ID);
        std::unique_ptr<PacketBodyV1> packetBody = nullptr;
        if(userInput == "QUIT") {
            std::cout << "Sending a QUIT packet to suspend the application" << std::endl;
            packetBody = std::make_unique<QuitPacketBodyV1>();
            inputHandlerRunning = false;
        } else {
            packetBody = std::make_unique<AsciiPacketBodyV1>(userInput);
        }

        auto packet = std::make_unique<PacketV1>(std::move(packetHeader), std::move(packetBody));
        comms->sendPacket(std::move(packet));

        if(!inputHandlerRunning) comms->stop();
    }
}

int main() {
    std::unique_ptr<Comms> comms;

    std::atomic<bool> inputHandlerRunning = true;

    MessageHandler messageHandler = [&inputHandlerRunning, &comms](const auto& packet) mutable {
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
                    inputHandlerRunning = false;
                    comms->stop();
                    break;
            }
        }
    };

#ifdef SERVER
    comms = std::make_unique<Server>(SERVER_PORT, messageHandler);
#else
    comms = std::make_unique<Client>("127.0.0.1", SERVER_PORT, messageHandler);
#endif

    std::thread inputHandlerThread(inputHandlerThreadFunction, std::ref(inputHandlerRunning), std::ref(comms));

    comms->block();
    if(inputHandlerThread.joinable()) inputHandlerThread.join();
}
#endif
