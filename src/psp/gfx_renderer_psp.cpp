//written by spicyjpeg
#ifdef PSP

#include <pspgu.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <cstring>
#include "gfx_common.h"
#include "gfx_renderer_psp.h"
#include "util.h"
#include "../main.h"

namespace Gfx {

PSPTexture::PSPTexture(TextureFormat _format, bool _bilinearFilter, int _width, int _height) {
	format         = _format;
	bilinearFilter = _bilinearFilter;

	width        = _width;
	height       = _height;
	bufferWidth  = roundToPowerOf2(_width);
	bufferHeight = roundToPowerOf2(_height);

	ptr = allocateAligned<uint8_t>(getBufferSize(), 16);
	ASSERTFUNC(ptr, "failed to allocate texture");
}

PSPTexture::~PSPTexture(void) {
	delete[] ptr;
}

void PSPTexture::loadData(const void *data) {
	// Swizzle the texture (reorder the pixels) so that it can be accessed
	// faster by the GPU.
	auto source = reinterpret_cast<const uint32_t *>(data);

	size_t blockOffset = 0;
	size_t blocksPerRow = 0;

	switch (format) {
		case TEXTURE_FMT_RGBA_32BPP: // 1 pixel -> 32 bits
			blocksPerRow = bufferWidth * sizeof(Color); 
			break;
		case TEXTURE_FMT_BC1: // 16 pixels -> 64 bits
			blocksPerRow = bufferWidth / 4;
			break;
		case TEXTURE_FMT_BC2: // 16 pixels -> 128 bits           
			[[fallthrough]];
		case TEXTURE_FMT_BC3:
			blocksPerRow = bufferWidth / 8;
			break;
		default:
			ASSERTFUNC(false, "invalid texture format");
			break;
	}

	blocksPerRow /= 16;

	for (int y = 0; y < bufferHeight; y++) {
		auto block = reinterpret_cast<uint32_t *>(ptr + blockOffset);

		for (int x = 0; x < static_cast<int>(blocksPerRow); x++) {
			*block++ = *source++;
			*block++ = *source++;
			*block++ = *source++;
			*block++ = *source++;

			block += 28;
		}

		if ((y % 8) == 7)
			blockOffset += (blocksPerRow - 1) * 128;

		blockOffset += 16;
	}

	sceKernelDcacheWritebackRange(ptr, getBufferSize());
	sceGuTexFlush();
}

PSPRenderer::PSPRenderer(int width, int height) : Renderer(width, height), _currentDisplayList(false) {
	_displayLists[0] = allocateAligned<uint8_t>(DISPLAY_LIST_LENGTH, 16);
	_displayLists[1] = allocateAligned<uint8_t>(DISPLAY_LIST_LENGTH, 16);

	ASSERTFUNC(_displayLists[0] && _displayLists[1], "failed to allocate display list");

	// The "real" framebuffer width must always be a power of 2.
	int bufferWidth = roundToPowerOf2(width);
	int bufferSize  = width * height * sizeof(Color);

	uintptr_t buffer1Ptr     = 0;
	uintptr_t buffer2Ptr     = buffer1Ptr + bufferSize;
	uintptr_t depthBufferPtr = buffer2Ptr + bufferSize;

	sceGuInit();
	beginFrame();

	sceGuDrawBuffer(GU_PSM_8888, reinterpret_cast<void *>(buffer1Ptr), bufferWidth);
	sceGuDispBuffer(width, height, reinterpret_cast<void *>(buffer2Ptr), bufferWidth);
	sceGuDepthBuffer(reinterpret_cast<void *>(depthBufferPtr), bufferWidth);

	int center = 2048;

	sceGuOffset(center - width / 2, center - height / 2);
	sceGuViewport(center, center, width, height);
	sceGuDepthRange(0xffff, 0);

	sceGuScissor(0, 0, width, height);
	sceGuEnable(GU_SCISSOR_TEST);

	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);

	sceGuShadeModel(GU_SMOOTH);
	sceGuDisable(GU_CULL_FACE);
	sceGuEnable(GU_CLIP_PLANES);

	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexMapMode(GU_TEXTURE_COORDS, 0, 0);
	sceGuTexWrap(GU_REPEAT, GU_REPEAT);

	endFrame();
	sceGuDisplay(GU_TRUE);

	// Reset all GPU matrices.
	[[gnu::aligned(16)]] static Mat4 mat;
	auto matPtr = reinterpret_cast<const ScePspFMatrix4 *>(&mat);

	mat.setIdentity();
	sceGuSetMatrix(GU_MODEL, matPtr);
	sceGuSetMatrix(GU_VIEW, matPtr);
	sceGuSetMatrix(GU_PROJECTION, matPtr);
}

PSPRenderer::~PSPRenderer(void) {
	endFrame();
	//sceGuDisplay(GU_FALSE);
	sceGuTerm();

	delete[] _displayLists[0];
	delete[] _displayLists[1];
}

void PSPRenderer::beginFrame(void) {
	auto list = _displayLists[_currentDisplayList];
	_currentDisplayList = !_currentDisplayList;

	sceKernelDcacheWritebackRange(list, DISPLAY_LIST_LENGTH);
	sceGuStart(GU_DIRECT, list);
}

void PSPRenderer::endFrame(void) {
	sceGuFinish();
	sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
}

void PSPRenderer::swapBuffers(void) {
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

void *PSPRenderer::allocatePrimData(size_t length) {
	return sceGuGetMemory(length);
}

void PSPRenderer::setMatrix(const Mat4 &mat) {    
//	sceGuSetMatrix(GU_MODEL, reinterpret_cast<const ScePspFMatrix4 *>(&mat));
}

void PSPRenderer::setTexture(const Texture &tex) {
/*
	ASSERTFUNC(tex.ptr, "attempted to use unallocated texture");

	int psm;

	switch (tex.format) {
		case TEXTURE_FMT_RGBA_32BPP:
			psm = GU_PSM_8888;
			break;

		case TEXTURE_FMT_BC1:
			psm = GU_PSM_DXT1;
			break;

		case TEXTURE_FMT_BC2:
			psm = GU_PSM_DXT3;
			break;

		case TEXTURE_FMT_BC3:
			psm = GU_PSM_DXT5;
			break;

		default:
			ASSERTFUNC(false, "invalid texture format");
	}

	int filter = tex.bilinearFilter ? GU_LINEAR : GU_NEAREST;

	sceGuTexMode(psm, 0, 0, GU_TRUE);
	sceGuTexFilter(filter, filter);
	sceGuTexImage(0, tex.bufferWidth, tex.bufferHeight, tex.width, tex.ptr);*/
}

void PSPRenderer::clear(Color color, int z) {
	sceGuClearColor(color.value);
	sceGuClearDepth(z);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT | GU_DEPTH_BUFFER_BIT);
}

void PSPRenderer::drawPoints(const Point *prims, size_t count) {
	int flags = 0
		| GU_COLOR_8888    // 24bpp vertex colors
		| GU_VERTEX_32BITF // float vertices
		| GU_TRANSFORM_3D; // use matrices

	sceGuDisable(GU_TEXTURE_2D);
	sceGuDrawArray(GU_POINTS, flags, count, nullptr, prims); // 1 vertex per primitive
}

void PSPRenderer::drawLines(const Line *prims, size_t count) {
	int flags = 0
		| GU_COLOR_8888    // 24bpp vertex colors
		| GU_VERTEX_32BITF // float vertices
		| GU_TRANSFORM_3D; // use matrices

	sceGuDisable(GU_TEXTURE_2D);
	sceGuDrawArray(GU_LINES, flags, count * 2, nullptr, prims); // 2 vertices per primitive
}

void PSPRenderer::drawTriangles(const Triangle *prims, size_t count) {
	int flags = 0
		| GU_COLOR_8888    // 24bpp vertex colors
		| GU_VERTEX_32BITF // float vertices
		| GU_TRANSFORM_3D; // use matrices

	sceGuDisable(GU_TEXTURE_2D);
	sceGuDrawArray(GU_TRIANGLES, flags, count * 3, nullptr, prims); // 3 vertices per primitive
}

void PSPRenderer::drawTexturedTriangles(const TexturedTriangle *prims, size_t count) {
	int flags = 0
		| GU_TEXTURE_16BIT // 16-bit UVs
		| GU_COLOR_8888    // 24bpp vertex colors
		| GU_VERTEX_32BITF // float vertices
		| GU_TRANSFORM_3D; // use matrices

	sceGuEnable(GU_TEXTURE_2D);
	sceGuDrawArray(GU_TRIANGLES, flags, count * 3, nullptr, prims); // 3 vertices per primitive
}

}

#endif
