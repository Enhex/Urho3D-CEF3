#ifndef MY_APP_HPP
#define MY_APP_HPP


#include "Application.h"
#include "Engine.h"
#include "InputEvents.h"
#include "PhysicsEvents.h"
#include "Graphics.h"
#include "Renderer.h"
#include "Scene.h"

#include "include/cef_app.h"

#include "TextureClient.hpp"

using namespace Urho3D;


class MyApp : public Application
{
public:
	MyApp(Context* context);

	SharedPtr<Scene> scene;
	// Camera scene node
	SharedPtr<Node> cameraNode;

	/// Camera yaw angle.
	float yaw_ = 0;
	/// Camera pitch angle.
	float pitch_ = 0;

	CefRefPtr<TextureRenderHandler> renderTextureHandler;
	CefRefPtr<TextureClient> textureClient;
	CefRefPtr<CefBrowser> browser;


	void Start();

	void Stop();

	void StartCef();

	void CreateScene();

	void SetupViewport();

	void MoveCamera(float timeStep);

	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandlePhysicsPrestep(StringHash eventType, VariantMap& eventData);
};


#endif//MY_APP_HPP