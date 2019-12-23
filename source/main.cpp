#include <iostream>
#include <string_view>

#include <sdl2/SDL.h>
#ifdef main
	#undef main
#endif
#include <sdl2/SDL_ttf.h>

#include "helper.h"

#define CheckCall(theCall) (theCall == 0)

SDL_Event gEvent;

void DrawCross(SDL_Renderer* pRenderer, SDL_Point pAt, int pSize);

bool Update();


void UpdateFPS(SDL_Renderer* pRenderer, int fps, TTF_Font* pFont);
void RenderFPS(SDL_Renderer* pRenderer);

struct TextRendering
{
	SDL_Texture* theTexture;
	SDL_Rect theSize;	
};
TextRendering gFPSTexture{ nullptr, SDL_Rect{ 0, 0, 0, 0} };

void RenderText(TextRendering& pText, SDL_Renderer* pRenderer, const std::string_view& pMessage, TTF_Font* pFont, SDL_Color theColour);

int main()
{
	const auto success = CheckCall(SDL_Init(SDL_INIT_VIDEO));
	if (success)
	{
		SDL_Window* theWindow = SDL_CreateWindow(
			"frog",
			0,
			0,
			1024,
			768,
			SDL_WINDOW_SHOWN);

		if (theWindow)
		{
			constexpr const auto targetFPS{ 30 };
			constexpr const auto msInSeconds{ 1000 };
			constexpr const auto targetSleep{msInSeconds / targetFPS };
			constexpr const auto defaultSleep{ std::chrono::milliseconds(targetSleep) };

			// Create Renderer
			SDL_Renderer* theRenderer = SDL_CreateRenderer(
				theWindow,
				-1,
				SDL_RENDERER_ACCELERATED);

			if (theRenderer)
			{
				const auto fontInitSuccess = CheckCall(TTF_Init());
				TTF_Font* theFont{ nullptr };
				if (fontInitSuccess)
				{
					theFont = TTF_OpenFont(
						xelous::GetFontPath().string().c_str(),
						14);
				}

				int frameCount{ 0 };
				xelous::TimePoint frameStart, frameEnd, lastFPSUpdate;
				bool firstFrame{ true };
				bool running{ true };
				int fps{ 0 };
				
				while (running)
				{
					frameStart = xelous::Now();
					if (firstFrame)
					{
						firstFrame = false;
						lastFPSUpdate = xelous::Now();
					}
					else
					{
						const auto sinceLastFPSUpdate = xelous::Now() - lastFPSUpdate;
						const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(sinceLastFPSUpdate);
						if (duration.count() >= msInSeconds)
						{
							fps = frameCount;
							frameCount = 0;
							// Display this
							//xelous::Log(std::to_string(fps));

							lastFPSUpdate = xelous::Now();

							UpdateFPS(theRenderer, fps, theFont);
						}						
					}
					++frameCount;

					// process user input
					running = Update();


					SDL_SetRenderDrawColor(theRenderer, 79, 94, 233, 255);
					SDL_RenderClear(theRenderer);

					SDL_SetRenderDrawColor(theRenderer, 0, 255, 0, 255);
					DrawCross(theRenderer,
						SDL_Point{ 1024 / 2, 768 / 2 },
						5);

					// Draw FPS
					RenderFPS(theRenderer);

					SDL_RenderPresent(theRenderer);

					frameEnd = xelous::Now();
					// Calculate real time taken and deduct from default sleep
					// to sleep for less and even out the frame time
					const auto timeTaken = frameEnd - frameStart;
					const auto takenDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timeTaken);
					if (takenDuration.count() < targetSleep)
					{
						const auto actualSleep = targetSleep - takenDuration.count();											
						xelous::SleepFor(std::chrono::milliseconds(actualSleep));
					}
					else
					{
						xelous::Log("No Sleep time - stall");
					}
				}

				if (fontInitSuccess)
				{
					if (theFont)
					{
						TTF_CloseFont(theFont);
					}
					TTF_Quit();
				}

				SDL_DestroyRenderer(theRenderer);
			}
			else
			{
				xelous::Log("Failed to create Renderer");
			}

			SDL_DestroyWindow(theWindow);
		}
		else
		{
			xelous::Log("Failed to create Window");
		}

		SDL_Quit();
	}
	else
	{
		xelous::Log("Failed to initialise SDL");
	}
}

void DrawCross(SDL_Renderer* pRenderer, SDL_Point pAt, int pSize)
{
	SDL_RenderDrawLine(
		pRenderer,
		pAt.x - pSize,
		pAt.y - pSize,
		pAt.x + pSize,
		pAt.y + pSize);
	SDL_RenderDrawLine(
		pRenderer,
		pAt.x + pSize,
		pAt.y - pSize,
		pAt.x - pSize,
		pAt.y + pSize);
}

bool Update()
{
	if (SDL_PollEvent(&gEvent))
	{
		switch (gEvent.type)
		{
		case SDL_QUIT:
			return false;
			break;
		}
	}
	return true;
}

void UpdateFPS(SDL_Renderer* pRenderer, int fps, TTF_Font* pFont)
{
	const auto value = std::to_string(fps);
	RenderText(gFPSTexture, pRenderer, value, pFont, { 255, 255, 255, 255 });
}

void RenderFPS(SDL_Renderer* pRenderer)
{
	if (gFPSTexture.theTexture)
	{		
		SDL_RenderCopy(
			pRenderer,
			gFPSTexture.theTexture,
			nullptr,
			&gFPSTexture.theSize);
	}
}

void RenderText(TextRendering& pText, SDL_Renderer* pRenderer, const std::string_view& pMessage, TTF_Font* pFont, SDL_Color theColour)
{	
	if (pRenderer && pFont && !pMessage.empty())
	{
		SDL_Surface* theSurface = TTF_RenderText_Solid(
			pFont,
			pMessage.data(),
			theColour);
		if (theSurface)
		{
			if (pText.theTexture)
			{
				//SDL_DestroyTexture(pText.theTexture);
			}

			pText.theTexture = SDL_CreateTextureFromSurface(
				pRenderer,
				theSurface);
			pText.theSize = SDL_Rect{ 0, 0, theSurface->w, theSurface->h };

			SDL_FreeSurface(theSurface);
		}
	}	
}