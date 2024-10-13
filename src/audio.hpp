enum SoundEffect{
    NULL_SOUND,
    CANNON,
    MUSKET_VOLLEY,
    MAX_VALUE
};
void initAudio();
unsigned int createAudioSource(SoundEffect);
