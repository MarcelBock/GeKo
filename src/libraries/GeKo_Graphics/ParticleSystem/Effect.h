#pragma once
#include "GeKo_Graphics/ParticleSystem/Emitter.h"
#include "tinyxml2.h"

/*
Compilation of different Emitters.
*/
class Effect{
public:
	Effect();	//creates an Effect with no Emitters
	Effect(const char* filepath);	//creates an Effect loaded from a XML file
	~Effect(); //deletes the Effect

	/*void active();*/

	void addEmitter(Emitter* emitter);	//the Emitter pointer will be stored in a vector of emitter pointers.
	void removeEmitter(int vectorPosition);	//if it's the first Emitter you added, it's at position 0 and so on

	void updateEmitters(Camera &cam);	//compute Shader
	void renderEmitters(Camera &cam);	//render Shader

	int loadEffect(const char* filepath);	//load an effect from an XML file and replace the current Effect
	int saveEffect(char* filepath);		//save the settings of this effect to a XML file

	void setPosition(glm::vec3 newPosition);	//updates the positions of every Emitter

private:
	std::vector<Emitter*> emitterVec;	//contains all Emitters of the Effect

	//check for a XML Error. Gives feedback what went wrong if something goes wrong
	int XMLCheckResult(int result);
};