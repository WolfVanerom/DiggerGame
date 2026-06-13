#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Renderer.h"
#include "SceneManager.h"
#include "Texture2D.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

void dae::Renderer::Init(SDL_Window* window)
{
	m_window = window;
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	m_renderer = SDL_CreateRenderer(window, nullptr);
	if (m_renderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#if __EMSCRIPTEN__
	io.IniFilename = NULL;
#endif

	ImGui_ImplSDL3_InitForSDLRenderer(window, m_renderer);
	ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void dae::Renderer::Render() const
{
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	SceneManager::GetInstance().RenderOneScene();

	ImGui::Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
	SDL_RenderPresent(m_renderer);
}

void dae::Renderer::Destroy()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, const float srcX, const float srcY, const float srcWidth, const float srcHeight) const
{
	SDL_FRect src{};
	src.x = srcX;
	src.y = srcY;
	src.w = srcWidth;
	src.h = srcHeight;

	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;

	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), &src, &dst);
}

void dae::Renderer::RenderTextureRotated(const Texture2D& texture, float x, float y, float width, float height, bool flipHorizontal, double angle, bool flipVertical) const
{
	SDL_FlipMode flip = SDL_FLIP_NONE;

	if (flipHorizontal) {
		flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
	}

	if (flipVertical) {
		flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
	}

	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;

	SDL_RenderTextureRotated(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, angle, nullptr, flip);
}

void dae::Renderer::RenderRect(const float x, const float y, const float width, const float height, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) const
{
	SDL_SetRenderDrawColor(GetSDLRenderer(), r, g, b, a);
	SDL_FRect rect{};
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	SDL_RenderFillRect(GetSDLRenderer(), &rect);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_renderer; }

const SDL_Color& dae::Renderer::GetBackgroundColor() const { return m_clearColor; }
