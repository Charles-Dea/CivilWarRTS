#include<GLFW/glfw3.h>
//the renderer doesn't rely on network.hpp I'm just trying to fix stupid class redefinition erros from header files
#include"network.hpp"
//textures are rendered in the order that they are listed below requiring semi-transparent textures be below fully opaque ones
enum Texture{
    NULL_TEXTURE,
    CS_INF_TEXTURE_UNSEL,
    CS_INF_TEXTURE_SEL,
    US_INF_TEXTURE_UNSEL,
    US_INF_TEXTURE_SEL,
    CS_BRIG_TEXTURE_UNSEL,
    CS_BRIG_TEXTURE_SEL,
    US_BRIG_TEXTURE_UNSEL,
    US_BRIG_TEXTURE_SEL,
    CS_ART_TEXTURE_UNSEL,
    CS_ART_TEXTURE_SEL,
    US_ART_TEXTURE_UNSEL,
    US_ART_TEXTURE_SEL,
    CS_CAV_TEXTURE_UNSEL,
    CS_CAV_TEXTURE_SEL,
    US_CAV_TEXTURE_UNSEL,
    US_CAV_TEXTURE_SEL,
    FIRING_INF_TEXTURE_UNSEL,
    FIRING_INF_TEXTURE_SEL,
    FIRING_ART_TEXTURE_UNSEL,
    FIRING_ART_TEXTURE_SEL,
    FIRING_CAV_TEXTURE_UNSEL,
    FIRING_CAV_TEXTURE_SEL,
    BOX_TEXTURE,
    CS_FLAG,
    US_FLAG,
    ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	HOST_GAME_BUTTON,
	JOIN_GAME_BUTTON,
	EXIT_GAME_BUTTON,
    WAITING_FOR_PLAYER,
    DOT,
    CSA_WINS,
    USA_WINS,
    COULD_NOT_CONNECT,
    SELECT_SIDE,
    ENTER_HOST_IP_ADDRESS,
    Y,
    N,
    MAXIMUM_VALUE
};
GLFWwindow*initRenderer(float*);
//updateRenderer will automatically unpause the renderer
void updateRenderer(
        Regiment*(*)[],
        unsigned short,
        Regiment*(*)[],
        unsigned short,
        Sprite*(*)[],
        unsigned short,
        Scene,
        Hill(*)[],
        unsigned short
        );
//pauseRendering does not return until current frame is finished
void pauseRendering();
void unPauseRendering();
void setBackground(float,float,float);
