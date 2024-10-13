#include<enet/enet.h>
#include<iostream>
#include"network.hpp"
using namespace std;
NetworkStatus status=disconnected;
ENetHost*host=nullptr;
ENetPeer*peer=nullptr;
ENetEvent event;
Unit*(*infRegiments)[]=nullptr;
void setupHost(){
    //atexit(enet_deinitialize);
    ENetAddress address;
    address.host=ENET_HOST_ANY;
    address.port=7777;
    if(host)
        enet_host_destroy(host);
    host=enet_host_create(&address,1,1,0,0);
    if(!host)
        printf("ERROR: failed to start host\n");
    status=waiting;
}
NetworkStatus listenOnPort(bool*isCs,bool*isCsSet){
    if(status==disconnected)
        return disconnected;
    while(enet_host_service(host,&event,1)>0){
        if(status==waiting&&event.type==ENET_EVENT_TYPE_CONNECT){
            peer=event.peer;
            bool isOponentCs=!*isCs;
            ENetPacket*packet=enet_packet_create(&isOponentCs,sizeof(bool),ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer,0,packet);
            status=connected;
            break;
        }else if(status==connected&&event.type==ENET_EVENT_TYPE_RECEIVE){
            if(event.packet->dataLength==sizeof(bool)){
                *isCs=*(bool*)event.packet->data;
                *isCsSet=true;
            }else if(event.packet->dataLength==sizeof(float[4])){
                float(*data)[4]=(float(*)[4])event.packet->data;
                unsigned short index=(*data)[0];
                (*(*infRegiments)[index].*(*infRegiments)[index]->sendOrder)
                    (static_cast<OrderType>((*data)[3]),(*data)[1],(*data)[2]);
            }else if(event.packet->dataLength==sizeof(unsigned short)){
                unsigned short*i=(unsigned short*)event.packet->data;
                (*(*infRegiments)[*i].*(*infRegiments)[*i]->sendOrder)(halt,NULL,NULL);
            }else if(event.packet->dataLength==sizeof(SetLimbered)){
                SetLimbered*data=(SetLimbered*)event.packet->data;
                if((*infRegiments)[data->i]){
                    if((*infRegiments)[data->i]->type==artillery_battery){
                        ArtilleryBattery*battery=(ArtilleryBattery*)(*infRegiments)[data->i];
                        battery->limbered=data->limbered;
                    }else{
                        CavalryRegiment*reg=(CavalryRegiment*)(*infRegiments)[data->i];
                        reg->mounted=data->limbered;
                    }
                }
            }
            enet_packet_destroy(event.packet);
            break;
        }else if(event.type==ENET_EVENT_TYPE_DISCONNECT){
            status=disconnected;
            enet_peer_reset(peer);
            //enet_host_destroy(host);
            *isCsSet=false;
        }
    }
    return status;
}
bool setupGuest(Unit*(*regs)[],char*ipAddress){
    //atexit(enet_deinitialize);
    host=enet_host_create(NULL,1,1,0,0);
    if(!host){
        printf("ERROR: failed to create enet host\n");
        return false;
    }
    ENetAddress ip;
    enet_address_set_host(&ip,ipAddress);
    ip.port=7777;
    peer=enet_host_connect(host,&ip,1,0);
    if(!host){
        printf("ERROR: failed to connect to host\n");
        return false;
    }
    if(enet_host_service(host,&event,1000)<=0||event.type!=ENET_EVENT_TYPE_CONNECT){
        printf("ERROR: failed to connect to host\n");
        return false;
    }
    status=connected;
    infRegiments=regs;
    return true;
}
void sendOrder(float(*data)[4]){
    ENetPacket*packet=enet_packet_create(data,sizeof(float[4]),ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer,0,packet);
}
void setRegs(Unit*(*regs)[]){
    infRegiments=regs;
}
void disconnect(){
    if(status==connected){
        enet_peer_disconnect(peer,0);
        while(enet_host_service(host,&event,1000)>0){
            if(event.type==ENET_EVENT_TYPE_RECEIVE){
                enet_packet_destroy(event.packet);
            }else if(event.type==ENET_EVENT_TYPE_DISCONNECT){
                enet_host_destroy(host);
                host=nullptr;
                return;
            }
        }
        printf("ERROR: diconnect failed\n");
        enet_peer_reset(peer);
    }else if(status==waiting){
        enet_host_destroy(host);
        host=nullptr;
    }
}
void orderHalt(unsigned short*i){
    ENetPacket*packet=enet_packet_create(i,sizeof(unsigned short),ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer,0,packet);
}
void setLimbered(SetLimbered*data){
    ENetPacket*packet=enet_packet_create(data,sizeof(SetLimbered),ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer,0,packet);
}
