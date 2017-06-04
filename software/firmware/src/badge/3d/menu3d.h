#ifndef MENU3D
#define MENU3D

#include "../menus.h"
#include "renderer.h"

class Model;

class Menu3D : public StateBase {
public:
	Menu3D();
	virtual ~Menu3D();
public:
protected:
	virtual ErrorType onInit(RunContext &rc);
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
	void initMenu3d(RunContext &rc);
	void update(RunContext &rc);
	void render(RunContext &rc);
private:
	Model model;
	Vec3f light_dir;
	Vec3f eye;
	Vec3f center;
	Vec3f up;
	uint8_t CanvasWidth;
	uint8_t CanvasHeight;
};

#endif
