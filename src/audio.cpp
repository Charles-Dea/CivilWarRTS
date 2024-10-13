#include<AL/al.h>
#include<AL/alc.h>
#include<AL/alut.h>
#include<cstdint>
#include<iostream>
#include<cstring>
#include"audio.hpp"
using namespace std;
void initAudio(){
    ALCdevice*device=alcOpenDevice(nullptr);
    if(!device){
        printf("ERROR: could not open device\n");
        return;
    }
    if(!alcIsExtensionPresent(nullptr,"ALC_ENUMERATION_EXT")){
        printf("ERROR: openal does not support device enumeration\n");
        return;
    }
    if(!alcMakeContextCurrent(alcCreateContext(device,nullptr))){
        printf("ERROR: failed to make openal context current\n");
        return;
    }
    alListener3f(AL_POSITION,0.0f,0.0f,1.0f);
    alListener3f(AL_VELOCITY,0,0,0);
    ALfloat listenerOrientation[]={0.0f,0.0f,1.0f,0.0f,1.0f,0.0f};
    alListenerfv(AL_ORIENTATION,listenerOrientation);
    char*audioFiles[]={"cannon","musket_volley"};
    for(unsigned short count=0;count<MAX_VALUE-1;++count){
        ALuint buffer;
        alGenBuffers(1,&buffer);
        char filePath[sizeof("res/sfx/.wav")+strlen(audioFiles[count])];
        sprintf(filePath,"res/sfx/%s.wav",audioFiles[count]);
        ALsizei size,freq;
        ALenum format;
        ALvoid*data;
        ALboolean loop=false;
        alutLoadWAVFile((signed char*)filePath,&format,&data,&size,&freq,&loop);
        alBufferData(buffer,format,data,size,freq);
    }
}
unsigned int createAudioSource(SoundEffect sound){
    ALuint source;
    alGenSources(1,&source);
    alSourcef(source,AL_PITCH,1);
    alSourcef(source,AL_GAIN,1);
    alSource3f(source,AL_POSITION,0.0f,0.0f,0.0f);
    alSource3f(source,AL_VELOCITY,0.0f,0.0f,0.0f);
    alSourcei(source,AL_LOOPING,AL_FALSE);
    alSourcei(source,AL_BUFFER,sound);
    return source;
}
