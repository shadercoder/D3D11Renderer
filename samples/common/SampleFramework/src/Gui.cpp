//
// This file is pretty much copy paste from the ImGui OpenGL 2.0 sample
//
#include "Gui.h"
#include "ImGui/imgui.h"
#include <Windows.h>
#include <Gl/GL.h>
#include <SampleFramework/Input.h>

static void renderImGui(ImDrawList** const cmd_lists, int cmd_lists_count) {
	if (cmd_lists_count == 0)
		return;

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);

	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	for (int n = 0; n < cmd_lists_count; n++) {
		const ImDrawList* cmd_list = cmd_lists[n];
		const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->vtx_buffer.front();
		glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));
		int vtx_offset = 0;
		for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++) {
			const ImDrawCmd* pcmd = &cmd_list->commands[cmd_i];
			if (pcmd->user_callback) {
				pcmd->user_callback(cmd_list, pcmd);
			} else {
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
				glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
				glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
			}
			vtx_offset += pcmd->vtx_count;
		}
	}
	#undef OFFSETOF
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

static GLuint gFontTexture {0};
static float gWidth;

bool SampleFramework::Gui::initialize() {
	ImGuiIO& io = ImGui::GetIO();
	//io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                 // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.

	io.RenderDrawListsFn = renderImGui;
	//io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
	//io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
	//io.ImeWindowHandle = glfwGetWin32Window(g_Window);
	io.DisplaySize = ImVec2(300.0f, 640.0f);

	// Build texture
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	// Create texture
	glGenTextures(1, &gFontTexture);
	glBindTexture(GL_TEXTURE_2D, gFontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)gFontTexture;
	return true;
}

void SampleFramework::Gui::resize(const int width, const int height) {
	ImGuiIO& io = ImGui::GetIO();
	float fw = static_cast<float>(width);
	float fh = static_cast<float>(height);
	io.DisplaySize = ImVec2(fw, fh);
	glViewport(0, 0, width, height);
}

void SampleFramework::Gui::newFrame() {
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	int cursorX, cursorY;
	SDL_GetMouseState(&cursorX, &cursorY);
	io.MousePos = 
		ImVec2(static_cast<float>(cursorX), static_cast<float>(cursorY));
}

void SampleFramework::Gui::render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Render();
}

void SampleFramework::Gui::handleEvent(const SDL_Event& event) {
	ImGuiIO& io = ImGui::GetIO();
	switch(event.type) {
	case SDL_QUIT:
		break;
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
			resize(event.window.data1, event.window.data2);
		}
		break;
	case SDL_KEYDOWN:
		break;
	case SDL_KEYUP:
		break;
	case SDL_MOUSEBUTTONDOWN:
		io.MouseDown[0] = true;
		break;
	case SDL_MOUSEBUTTONUP:
		io.MouseDown[0] = false;
		break;
	}
}

void SampleFramework::Gui::terminate() {
	glDeleteTextures(1, &gFontTexture);
	ImGui::Shutdown();
}
