#include "raknet_connector.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <cstring>

using namespace godot;

void RakNetConnector::_bind_methods() {
    ClassDB::bind_method(D_METHOD("startup", "port", "max_clients"), &RakNetConnector::startup, DEFVAL(0), DEFVAL(1));
    ClassDB::bind_method(D_METHOD("shutdown", "time_ms"), &RakNetConnector::shutdown, DEFVAL(1000));
    ClassDB::bind_method(D_METHOD("connect", "host", "port"), &RakNetConnector::connect);
    ClassDB::bind_method(D_METHOD("send", "packet"), &RakNetConnector::send);
    ClassDB::bind_method(D_METHOD("receive"), &RakNetConnector::receive);
}

RakNetConnector::RakNetConnector() {
    this->peer = RakNet::RakPeerInterface::GetInstance();
}

RakNetConnector::~RakNetConnector() {
    this->shutdown();
    RakNet::RakPeerInterface::DestroyInstance(this->peer);
}

void RakNetConnector::startup(int port, int max_clients) {
    RakNet::SocketDescriptor sd(port, 0);
    this->peer->Startup(max_clients, &sd, 1);
    this->peer->SetMaximumIncomingConnections(max_clients);
}

Error RakNetConnector::connect(String host, int port) {
    this->peer->Connect(host.ascii().get_data(), port, 0, 0);

    bool awaiting_answer = true;
    RakNet::Packet *temp_packet;
    while (awaiting_answer) {
        RakSleep(500);
        temp_packet = this->peer->Receive();
        if (!temp_packet) {
            continue;
        }
        uint8_t temp_packet_id;
        this->guid = temp_packet->guid;
        RakNet::BitStream temp_receive_stream(temp_packet->data, (int)temp_packet->bitSize, false);
        temp_receive_stream.Read<uint8_t>(temp_packet_id);
        if (temp_packet_id == ID_CONNECTION_REQUEST_ACCEPTED) {
            awaiting_answer = false;
        } else {
            if ((int)temp_packet_id == ID_CONNECTION_ATTEMPT_FAILED) {
                awaiting_answer = false;
                return Error::ERR_CANT_CONNECT;
            }
        }
    }
    return Error::OK;
}

void RakNetConnector::send(PackedByteArray packet) {
    RakNet::BitStream stream(packet.ptrw(), packet.size(), true); // Copy data
    this->peer->Send(&stream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, this->guid, false);
}

void RakNetConnector::shutdown(int time_ms) {
    this->peer->Shutdown(time_ms, 0, IMMEDIATE_PRIORITY);
}

PackedByteArray RakNetConnector::receive() {
    RakNet::Packet *packet;
    packet = this->peer->Receive();

    if (!packet) {
        PackedByteArray empty_buffer;
        return empty_buffer;
    }

    PackedByteArray buffer;
    buffer.resize(packet->length);
    std::memcpy(buffer.ptrw(), packet->data, packet->length);
    return buffer;
}
