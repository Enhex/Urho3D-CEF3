#ifndef TEXTURE_RENDER_HANDLER_HPP
#define TEXTURE_RENDER_HANDLER_HPP


#include "include/cef_render_handler.h"
#include "Texture2D.h"

using namespace Urho3D;


/*
Renders the browser to a texture.
*/
class TextureRenderHandler : public CefRenderHandler
{
public:
	TextureRenderHandler(Texture2D* texture) :
		texture(texture)
	{
	}

	SharedPtr<Texture2D> texture;

	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
	{
		rect = CefRect(0, 0, texture->GetWidth(), texture->GetHeight());
		return true;
	}

	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
	{
		texture->SetData(0, 0, 0, width, height, buffer);
	}


	IMPLEMENT_REFCOUNTING(TextureRenderHandler);
};


#endif//TEXTURE_RENDER_HANDLER_HPP