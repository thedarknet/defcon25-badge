#include <stdlib.h>
#include "stdint.h"
#include "renderer.h"
#include "menu3d.h"

/*
 * The model, view and projection matrices are three separate matrices. Model maps from an object's local coordinate
 * space into world space, view from world space to camera space, projection from camera to screen.
 *
 * https://en.wikipedia.org/wiki/Triangle_strip
 * static const GLfloat cubeVertices[] = {
    -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
    -1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,
    1.0,  1.0, -1.0,
};

static const GLushort cubeIndices[] = {
    0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};
 */

const struct VertexStruct Cube[] = {
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

const unsigned short CubeIndexes[] = {
		/* CUBE 12 faces */
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21,
		22, 20, 22, 23,
};

const Vec3f Menu3D::center(0, 0, 0);
const Vec3f Menu3D::up(0, 1, 0);

Menu3D::Menu3D() :
		model(), light_dir(), eye(), CanvasWidth(0), CanvasHeight(0) {
}

Menu3D::~Menu3D() {

}

enum INTERNAL_STATE {
	INIT, RENDER
};
static INTERNAL_STATE InternalState = INIT;
static float rotation = 0.0f;

ErrorType Menu3D::onInit(RunContext &rc) {
	//initGame();
	InternalState = INIT;
	model.set(&Cube[0], (sizeof(Cube) / sizeof(Cube[0])), &CubeIndexes[0],
			sizeof(CubeIndexes) / sizeof(CubeIndexes[0]));
	light_dir = Vec3f(1, 1, 1);
	eye = Vec3f(0, 2, 10);
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	rotation = 0.0f;
	return ErrorType();
}

void Menu3D::initMenu3d(RunContext &rc) {
	CanvasHeight = rc.getDisplay().getHeight() - 20;
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

static uint32_t renderTime = 0, count = 0, total_frames = 0;

void Menu3D::update(RunContext &rc) {
	UNUSED(rc);
	model.setTransformation(rotation);
	rotation += 0.05f;
}

void Menu3D::render(RunContext &rc) {
	rc.getDisplay().fillRec(0, 0, CanvasWidth, CanvasHeight, RGBColor::BLACK);
	uint8_t b[((CanvasWidth * CanvasHeight * 3) / 8) + 1] = { 0 };
	//memset(&b[0], 0, sizeof(b));
	BitArray zbuf(&b[0], CanvasWidth * CanvasHeight, 3);

	//GouraudShader shader;
	FlatShader shader;
	Matrix modelViewProj = Projection * ModelView * model.getModelTransform();
	shader.setLightDir(light_dir);
	{
		Vec3i screen_coords[3];

		for (uint32_t i = 0; i < model.nFaces(); i++) {
			////start backface cull
			Vec3f one = model.vert(i, 1) - model.vert(i, 0);
			Vec3f two = model.vert(i, 2) - model.vert(i, 0);
			Vec3f c = cross(one, two);
			Vec3f normalized = c.normalize();
			float dd = (normalized.x * one.x) + (normalized.y * one.y) + (normalized.z * one.z);
			//solve plane eq
			float solve = eye.x * normalized.x + eye.y * normalized.y + eye.z * normalized.z + dd;
			//end backface cull
			if (solve > 0.01f) {

				Vec2i bboxmin( INT16_MAX, INT16_MAX);
				Vec2i bboxmax(INT16_MIN, INT16_MIN);
				for (int j = 0; j < 3; j++) {
					screen_coords[j] = shader.vertex(modelViewProj, model, i, j);
					if (screen_coords[j].x < bboxmin.x)
						bboxmin.x = screen_coords[j].x < 0 ? 0 : screen_coords[j].x;
					if (screen_coords[j].y < bboxmin.y)
						bboxmin.y = screen_coords[j].y < 0 ? 0 : screen_coords[j].y;
					if (screen_coords[j].x > bboxmax.x)
						bboxmax.x = screen_coords[j].x > CanvasWidth ? CanvasWidth : screen_coords[j].x;
					if (screen_coords[j].y > bboxmax.y)
						bboxmax.y = screen_coords[j].y > CanvasHeight ? CanvasHeight : screen_coords[j].y;
				}
				//check for backface culling
				triangle(screen_coords, shader, zbuf, &rc.getDisplay(), bboxmin, bboxmax);
			}
		}
	}
	if (HAL_GetTick() - renderTime > 1000) {
		char buf[12];
		sprintf(&buf[0], "FPS: %u:%u", count, total_frames);
		rc.getDisplay().drawString(0, 140, &buf[0]);
		count = 0;
		renderTime = HAL_GetTick();
		//HAL_Delay(500);
	}
	++count;
	total_frames++;
}

