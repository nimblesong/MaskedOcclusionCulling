

#include <algorithm>

#include "MaskedOcclusionCulling.h"


static MaskedOcclusionCulling* g_OC_instance = nullptr;

extern "C" void MOC_Load(int width, int height, float near_plane)
{
	g_OC_instance = MaskedOcclusionCulling::Create();
	g_OC_instance->SetResolution(width, height);
	g_OC_instance->SetNearClipPlane(near_plane);
	LOGI("Init With Resolution(%d,%d,%f) ", width, height, near_plane);
}

extern "C" void MOC_Unload()
{
	MaskedOcclusionCulling::Destroy(g_OC_instance);
	g_OC_instance = nullptr;
}

extern "C" void MOC_ClearBuffer()
{
	if (g_OC_instance == nullptr)
		return;
	g_OC_instance->ClearBuffer();
}


extern "C" void MOC_RenderMesh(float* mesh_vertex_list, unsigned int mesh_vert_count, unsigned int* mesh_index_list, int tris_count, float* trans_matrix)
{
	if(g_OC_instance == nullptr)
		return;
	float *transed_vert_list = new float[mesh_vert_count];
	MaskedOcclusionCulling::TransformVertices(trans_matrix, mesh_vertex_list, transed_vert_list, mesh_vert_count);
	g_OC_instance->RenderTriangles(transed_vert_list, mesh_index_list, tris_count, trans_matrix);
	delete []transed_vert_list;
}

extern "C" int MOC_TestMesh(float* mesh_vertex_list, unsigned int mesh_vert_count, unsigned int* mesh_index_list, int tris_count, float* trans_matrix)
{
	if (g_OC_instance == nullptr)
		return -1;
	float* transed_vert_list = new float[mesh_vert_count];
	MaskedOcclusionCulling::TransformVertices(trans_matrix, mesh_vertex_list, transed_vert_list, mesh_vert_count);
	int result = g_OC_instance->TestTriangles(transed_vert_list, mesh_index_list, tris_count, trans_matrix);
	delete[]transed_vert_list;
	return result;
}

static void TonemapDepth(float* depth, unsigned char* image, int w, int h)
{
	// Find min/max w coordinate (discard cleared pixels)
	float minW = 10000, maxW = 0.0f;
	for (int i = 0; i < w * h; ++i)
	{
		if (depth[i] > 0.0f)
		{
			minW = std::min(minW, depth[i]);
			maxW = std::max(maxW, depth[i]);
		}
	}
	//LOGI("Get MinW %.2f, MaxW %.2f", minW, maxW);
	// Tonemap depth values
	for (int i = 0; i < w * h; ++i)
	{
		int intensity = 0;
		if (depth[i] > 0)
			intensity = (unsigned char)(223.0 * (depth[i] - minW) / (maxW - minW) + 32.0);

		image[i * 3 + 0] = intensity;
		image[i * 3 + 1] = intensity;
		image[i * 3 + 2] = intensity;
	}
}

static void WriteBMP(const char* filename, const unsigned char* data, int w, int h)
{
	short header[] = { 0x4D42, 0, 0, 0, 0, 26, 0, 12, 0, (short)w, (short)h, 1, 24 };
	FILE* f = fopen(filename, "wb");
	fwrite(header, 1, sizeof(header), f);
#if USE_D3D == 1
	// Flip image because Y axis of Direct3D points in the opposite direction of bmp. If the library 
	// is configured for OpenGL (USE_D3D 0) then the Y axes would match and this wouldn't be required.
	for (int y = 0; y < h; ++y)
		fwrite(&data[(h - y - 1) * w * 3], 1, w * 3, f);
#else
	fwrite(data, 1, w * h * 3, f);
#endif
	fclose(f);
}

static unsigned char* s_DebugImageData = nullptr;

extern "C" unsigned char* MOC_GetDepthMap()
{
	if (g_OC_instance == nullptr)
		return nullptr;
	unsigned int width, height = 0;
	
	g_OC_instance->GetResolution(width, height);
	//LOGI("Get width %d, height %d", width, height);
	// Compute a per pixel depth buffer from the hierarchical depth buffer, used for visualization.
	float* perPixelZBuffer = new float[width * height];
	g_OC_instance->ComputePixelDepthBuffer(perPixelZBuffer, false);
	int start_index = (width / 2) * (height / 2);
	//LOGI("Get depth center %f", perPixelZBuffer[start_index]);
	// Tonemap the image
	if(s_DebugImageData==nullptr)
	{
		s_DebugImageData = new unsigned char[width * height * 3];
	}
	TonemapDepth(perPixelZBuffer, s_DebugImageData, width, height);
	delete[]perPixelZBuffer;
	
	return s_DebugImageData;
}