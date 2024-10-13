struct Pair{
    float big;
    float small;
};
struct Hill{
    float x;
    float y;
    float height;
    float xSteepness;
    float ySteepness;
    float rotation;
    //getXs and getYs ignore rotation and are meant to be used by the renderer
    Pair getXs(float,float);
    Pair getYs(float,float);
    float getZ(float,float);
    Hill(float,float,float,float,float,float);
};
float hillGetZ(Hill(*)[],unsigned short,float,float);
