#include <stdlib.h>
#include "stdint.h"
#include "renderer.h"
#include "menu3d.h"


struct VertexStruct Cube[]={
	/* CUBE: 24 vertices */
	{{1.000000f, 1.000000f, -1.000000f},  {0.000000f, 0.000000f, -1.000000f},  {0.000000f, 0.000000f}},
	{{1.000000f, -1.000000f, -1.000000f}, {0.000000f, 0.000000f, -1.000000f},  {0.000000f, 0.000000f}},
	{{-1.000000f, -1.000000f, -1.000000f},{0.000000f, 0.000000f, -1.000000f},  {0.000000f, 0.000000f}},
	{{-1.000000f, 1.000000f, -1.000000f}, {0.000000f, 0.000000f, -1.000000f},  {0.000000f, 0.000000f}},
	{{1.000000f, 0.999999f, 1.000000f},   {0.000000f, -0.000000f, 1.000000f},  {0.000000f, 0.000000f}},
	{{-1.000000f, 1.000000f, 1.000000f},  {0.000000f, -0.000000f, 1.000000f},  {0.000000f, 0.000000f}},
	{{-1.000000f, -1.000000f, 1.000000f}, {0.000000f, -0.000000f, 1.000000f},  {0.000000f, 0.000000f}},
	{{0.999999f, -1.000001f, 1.000000f},  {0.000000f, -0.000000f, 1.000000f},  {0.000000f, 0.000000f}},
	{{1.000000f, 1.000000f, -1.000000f},  {1.000000f, -0.000000f, 0.000000f},  {0.000000f, 0.000000f}},
	{{1.000000f, 0.999999f, 1.000000f},   {1.000000f, -0.000000f, 0.000000f},  {0.000000f, 0.000000f}},
	{{0.999999f, -1.000001f, 1.000000f},  {1.000000f, -0.000000f, 0.000000f},  {0.000000f, 0.000000f}},
	{{1.000000f, -1.000000f, -1.000000f}, {1.000000f, -0.000000f, 0.000000f},  {0.000000f, 0.000000f}},
	{{1.000000f, -1.000000f, -1.000000f}, {-0.000000f, -1.000000f, -0.000000f},{0.000000f, 0.000000f}},
	{{0.999999f, -1.000001f, 1.000000f},  {-0.000000f, -1.000000f, -0.000000f},{0.000000f, 0.000000f}},
	{{-1.000000f, -1.000000f, 1.000000f}, {-0.000000f, -1.000000f, -0.000000f},{0.000000f, 0.000000f}},
	{{-1.000000f, -1.000000f, -1.000000f},{-0.000000f, -1.000000f, -0.000000f},{0.000000f, 0.000000f}},
	{{-1.000000f, -1.000000f, -1.000000f},{-1.000000f, 0.000000f, -0.000000f}, {0.000000f, 0.000000f}},
	{{-1.000000f, -1.000000f, 1.000000f}, {-1.000000f, 0.000000f, -0.000000f}, {0.000000f, 0.000000f}},
	{{-1.000000f, 1.000000f, 1.000000f},  {-1.000000f, 0.000000f, -0.000000f}, {0.000000f, 0.000000f}},
	{{-1.000000f, 1.000000f, -1.000000f}, {-1.000000f, 0.000000f, -0.000000f}, {0.000000f, 0.000000f}},
	{{1.000000f, 0.999999f, 1.000000f},   {0.000000f, 1.000000f, 0.000000f},   {0.000000f, 0.000000f}},
	{{1.000000f, 1.000000f, -1.000000f},  {0.000000f, 1.000000f, 0.000000f},   {0.000000f, 0.000000f}},
	{{-1.000000f, 1.000000f, -1.000000f}, {0.000000f, 1.000000f, 0.000000f},   {0.000000f, 0.000000f}},
	{{-1.000000f, 1.000000f, 1.000000f},  {0.000000f, 1.000000f, 0.000000f},   {0.000000f, 0.000000f}},
};

unsigned short CubeIndexes[]={
/* CUBE 12 faces */
0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23,
};

Vec3f light_dir(1, 1, 1);
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);


Menu3D::Menu3D() : model() {
}

Menu3D::~Menu3D() {

}

enum INTERNAL_STATE {
	INIT, RENDER
};
static INTERNAL_STATE InternalState = INIT;

ErrorType Menu3D::onInit() {
	//initGame();
	InternalState = INIT;
	model.set(&Cube[0],(sizeof(Cube)/sizeof(Cube[0])),&CubeIndexes[0], sizeof(CubeIndexes)/sizeof(CubeIndexes[0]));
	return ErrorType();
}

void Menu3D::initMenu3d(RunContext &rc) {
	UNUSED(rc);
}

ReturnStateContext Menu3D::onRun(RunContext &rc) {
	ReturnStateContext sr(this);
	switch (InternalState) {
	case INIT:
		initMenu3d(rc);
		InternalState = RENDER;
		break;
	case RENDER:
		update(rc);
		render(rc);
		break;
	}
	return sr;
}

ErrorType Menu3D::onShutdown() {
	return ErrorType();
}

void Menu3D::update(RunContext &rc) {
	UNUSED(rc);
}

void Menu3D::render(RunContext &rc) {
	lookat(eye, center, up);
	viewport(rc.getDisplay().getWidth() / 8, rc.getDisplay().getHeight() / 8, rc.getDisplay().getWidth() * 3 / 4, rc.getDisplay().getHeight() * 3 / 4);
	projection(-1.f / (eye - center).norm());
	light_dir.normalize();

	ZBuff zbuf(4,rc.getDisplay().getWidth(),rc.getDisplay().getHeight(),0);

	GouraudShader shader;
	for (uint32_t i = 0; i < model.nFaces(); i++) {
		Vec3i screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(model, i, j);
		}
		triangle(screen_coords, shader, zbuf, &rc.getDisplay());
	}

}

