#include "raknet_connector.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void RakNetConnector::_bind_methods() {
    ClassDB::bind_method(D_METHOD("startup", "port", "max_clients"), &RakNetConnector::startup, DEFVAL(0), DEFVAL(1));
}

RakNetConnector::RakNetConnector() {
    this->peer = RakNet::RakPeerInterface::GetInstance();
}

RakNetConnector::~RakNetConnector() {
    this->peer->Shutdown(1000, 0, IMMEDIATE_PRIORITY);
    RakNet::RakPeerInterface::DestroyInstance(this->peer);
}

void RakNetConnector::startup(int port, int max_clients) {
    RakNet::SocketDescriptor sd(port, 0);
    this->peer->Startup(max_clients, &sd, 1);
    this->peer->SetMaximumIncomingConnections(max_clients);
}
