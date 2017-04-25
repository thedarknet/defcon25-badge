#ifndef RENDERER_H
#define RENDERER_H

#include "vec_math.h"
#include "../display_device.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

struct VertexStruct {
	Vec3f pos;
	Vec3f normal;
	Vec2f uv;
};

class Model {
	//no model matrix to save space and computation we assume identity
public:
	Model();
	void set(VertexStruct *v, uint16_t nv, uint16_t *i, uint16_t ni);
	const Vec3f &normal(uint16_t face, uint8_t nVert) const;
	const Vec3f &vert(uint16_t face, uint8_t nVert) const;
	uint32_t nFaces() const;
	Matrix &getModelTransform() const {return ModelTransform;}
private:
	VertexStruct *Verts;
	uint16_t NumVerts;
	uint16_t *Indexes;
	uint8_t NumIndexes;
	Matrix ModelTransform;
};

class ZBuff {
public:
	ZBuff(uint8_t bitsPerPixel, uint8_t width, uint8_t height, uint8_t *buf);
	~ZBuff();
	bool set(uint16_t x, uint16_t y, uint16_t z);
	uint16_t get(uint16_t x, uint16_t y);
protected:
	uint16_t optimzedGet(uint32_t bitOffSet);
private:
	uint8_t *Buff;
	uint8_t BitsPerPixel;
	uint8_t Width;
	uint8_t Height;
};

class IShader {
public:
	IShader();
	virtual ~IShader() = 0;
	virtual Vec3i vertex(const Model &model, int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, RGBColor &color) = 0;
	void setLightDir(const Vec3f &ld);
	const Vec3f &getLightDir() const;
private:
	Vec3f LightDir;
};

class FlatShader: public IShader {
public:
	FlatShader();
	virtual ~FlatShader();

	virtual Vec3i vertex(const Model &model, int iface, int nthvert);
	virtual bool fragment(Vec3f bar, RGBColor &color);
private:
	mat<3, 3, float> varying_tri;
};

struct GouraudShader: public IShader {
public:
	GouraudShader();
	virtual ~GouraudShader();
	virtual Vec3i vertex(const Model &model, int iface, int nthvert);
	virtual bool fragment(Vec3f bar, RGBColor &color);
private:
	mat<3, 3, float> varying_tri;
	Vec3f varying_ity;
};

class ToonShader: public IShader {
public:
	ToonShader();
	virtual ~ToonShader();
	virtual Vec3i vertex(const Model &model, int iface, int nthvert);
	virtual bool fragment(Vec3f bar, RGBColor &color);
private:
	mat<3, 3, float> varying_tri;
	Vec3f varying_ity;

};

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);
void triangle(Vec3i *pts, IShader &shader, ZBuff &zbuffer, DisplayST7735 *display);

template<typename T> T CLAMP(const T& value, const T& low, const T& high) {
	return value < low ? low : (value > high ? high : value);
}

#endif
