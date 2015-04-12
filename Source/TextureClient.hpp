#ifndef TEXTURE_CLIENT_HPP
#define TEXTURE_CLIENT_HPP


#include "include/cef_client.h"
#include "TextureRenderHandler.hpp"


/*
Browser client that renders to texture.
*/
class TextureClient : public CefClient
{
public:
	CefRefPtr<CefRenderHandler> renderHandler;


	TextureClient(TextureRenderHandler* renderHandler) :
		renderHandler(renderHandler)
	{
	}

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
	{
		return renderHandler;
	}


	IMPLEMENT_REFCOUNTING(TextureClient);
};


#endif//TEXTURE_CLIENT_HPP