#include <stdlib.h>
#include "stdint.h"
#include "renderer.h"
#include "menu3d.h"

struct VertexStruct Cube[] = {
		/* CUBE: 24 vertices */
		{ { 1.000000f, 1.000000f, -1.000000f }, { 0.000000f, 0.000000f, -1.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, -1.000000f, -1.000000f }, { 0.000000f, 0.000000f, -1.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, -1.000000f, -1.000000f }, { 0.000000f, 0.000000f, -1.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, 1.000000f, -1.000000f }, { 0.000000f, 0.000000f, -1.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, 0.999999f, 1.000000f }, { 0.000000f, -0.000000f, 1.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, 1.000000f, 1.000000f }, { 0.000000f, -0.000000f, 1.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, -1.000000f, 1.000000f }, { 0.000000f, -0.000000f, 1.000000f }, { 0.000000f, 0.000000f } },
		{ { 0.999999f, -1.000001f, 1.000000f }, { 0.000000f, -0.000000f, 1.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, 1.000000f, -1.000000f }, { 1.000000f, -0.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, 0.999999f, 1.000000f }, { 1.000000f, -0.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { 0.999999f, -1.000001f, 1.000000f }, { 1.000000f, -0.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, -1.000000f, -1.000000f }, { 1.000000f, -0.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, -1.000000f, -1.000000f }, { -0.000000f, -1.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { 0.999999f, -1.000001f, 1.000000f }, { -0.000000f, -1.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, -1.000000f, 1.000000f }, { -0.000000f, -1.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, -1.000000f, -1.000000f }, { -0.000000f, -1.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, -1.000000f, -1.000000f }, { -1.000000f, 0.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, -1.000000f, 1.000000f }, { -1.000000f, 0.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, 1.000000f, 1.000000f }, { -1.000000f, 0.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, 1.000000f, -1.000000f }, { -1.000000f, 0.000000f, -0.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, 0.999999f, 1.000000f }, { 0.000000f, 1.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { 1.000000f, 1.000000f, -1.000000f }, { 0.000000f, 1.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, 1.000000f, -1.000000f }, { 0.000000f, 1.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
		{ { -1.000000f, 1.000000f, 1.000000f }, { 0.000000f, 1.000000f, 0.000000f }, { 0.000000f, 0.000000f } },
};

unsigned short CubeIndexes[] = {
		/* CUBE 12 faces */
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21,
		22, 20, 22, 23,
};

Menu3D::Menu3D() :
		model(),light_dir(),eye(), center(), up(), CanvasWidth(0), CanvasHeight(0) {
}

Menu3D::~Menu3D() {

}

enum INTERNAL_STATE {
	INIT, RENDER
};
static INTERNAL_STATE InternalState = INIT;

ErrorType Menu3D::onInit(RunContext &rc) {
	//initGame();
	InternalState = INIT;
	model.set(&Cube[0], (sizeof(Cube) / sizeof(Cube[0])), &CubeIndexes[0],
			sizeof(CubeIndexes) / sizeof(CubeIndexes[0]));
	light_dir=Vec3f(1, 1, 1);
	eye = Vec3f(0, 1, 6);
	center = Vec3f(0, 0, 0);
	up = Vec3f(0, 1, 0);
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	return ErrorType();
}

void Menu3D::initMenu3d(RunContext &rc) {
	CanvasHeight = rc.getDisplay().getHeight()-20;
	CanvasWidth = rc.getDisplay().getWidth();
	lookat(eye, center, up);
	viewport(CanvasWidth / 8, CanvasHeight / 8, CanvasWidth * 3 / 4,
			CanvasHeight * 3 / 4);
	projection(-1.f / (eye - center).norm());
	light_dir.normalize();
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

//static uint8_t b[(128*160)/2];
static uint32_t renderTime = 0, count = 0;

void Menu3D::render(RunContext &rc) {
	rc.getDisplay().fillRec(0,0,128,CanvasHeight,RGBColor::BLACK);
	//uint8_t b[(128*160)/2];
	uint8_t b[((CanvasWidth*CanvasHeight*3)/8)+1];
	memset(&b[0],0,sizeof(b));
	BitArray zbuf(&b[0],CanvasWidth*CanvasHeight,3);
	//ZBuff zbuf(4, CanvasWidth, CanvasHeight, &b[0]);
	//model.getModelTransform().

	GouraudShader shader;
	Matrix modelViewProj = Projection * ModelView;
	shader.setLightDir(light_dir);
	for (uint32_t i = 0; i < model.nFaces(); i++) {
		Vec3i screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(modelViewProj, model, i, j);
		}
		triangle(screen_coords, shader, zbuf, &rc.getDisplay());
	}
	if(HAL_GetTick()-renderTime>1000) {
		char buf[12];
		sprintf(&buf[0],"FPS: %u",count);
		rc.getDisplay().drawString(0,140,&buf[0]);
		count = 0;
		renderTime = HAL_GetTick();
		//HAL_Delay(500);
	}
	++count;
}

