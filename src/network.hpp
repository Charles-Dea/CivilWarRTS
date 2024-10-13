#include"input.hpp"
enum NetworkStatus{
    disconnected,
    waiting,
    connected
};
struct SetLimbered{
    unsigned short i;
    bool limbered;
};
void setupHost();
NetworkStatus listenOnPort(bool*,bool*);
bool setupGuest(Unit*(*)[],char*);
void sendOrder(float(*)[4]);
void setRegs(Unit*(*)[]);
void disconnect();
void orderHalt(unsigned short*);
void setLimbered(SetLimbered*);
