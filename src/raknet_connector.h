#ifndef GDRAKNET_H
#define GDRAKNET_H

#include <BitStream.h>
#include <MessageIdentifiers.h>
#include <RakPeerInterface.h>
#include <RakSleep.h>

#include <godot_cpp/classes/stream_peer_extension.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot {

class RakNetConnector : public Object {
    GDCLASS(RakNetConnector, Object)

private:
    RakNet::RakPeerInterface *peer;
    RakNet::RakNetGUID guid;
protected:
    static void _bind_methods();

public:
    RakNetConnector();
    ~RakNetConnector();

    void startup(int port = 0, int max_clients = 1);
    void shutdown(int time_ms = 1000);
    Error connect(String host, int port);
    void send(PackedByteArray packet);
    PackedByteArray receive();
};

}

#endif
