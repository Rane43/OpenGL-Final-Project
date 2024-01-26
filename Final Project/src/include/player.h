#ifndef player_h
#define player_h

#include <camera.h>


class Player {
private:
	Camera camera = Camera(800, 600);
	
	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	vec3 rotation = vec3(0.0f, 0.0f, 0.0f);
public:
	Player();

};


#endif 