#include "MyApp.hpp"

#include "Octree.h"
#include "PhysicsWorld.h"
#include "Camera.h"
#include "ResourceCache.h"
#include "Zone.h"
#include "Model.h"
#include "StaticModel.h"
#include "Technique.h"
#include "Material.h"
#include "Input.h"
#include "CoreEvents.h"


//
// Constructor
//
MyApp::MyApp(Context* context) :
Application(context)
{
	//CefMainArgs args(argc, argv);
	CefMainArgs args;
	{
		int result = CefExecuteProcess(args, nullptr, nullptr);
		// checkout CefApp, derive it and set it as second parameter, for more control on
		// command args and resources.
		if (result >= 0) // child proccess has endend, so exit.
		{
			//return;
		}
		if (result == -1)
		{
			// we are here in the father proccess.
		}
	}


	CefSettings settings;

	// checkout detailed settings options http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
	settings.windowless_rendering_enabled = true;
	//settings.single_process = true;
	//settings.multi_threaded_message_loop = true;

	bool result = CefInitialize(args, settings, nullptr, nullptr);
	// CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
	// if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
	if (!result)
	{
		// handle error
		//return;
	}
}


//
// Start
//
void MyApp::Start()
{
	// Create the scene content
	CreateScene();

	// Setup the viewport for displaying the scene
	SetupViewport();

	StartCef();

	SubscribeToEvent(E_KEYDOWN, HANDLER(MyApp, HandleKeyDown));
	SubscribeToEvent(E_UPDATE, HANDLER(MyApp, HandleUpdate));
	SubscribeToEvent(E_PHYSICSPRESTEP, HANDLER(MyApp, HandlePhysicsPrestep));
}


//
// Stop
//
void MyApp::Stop()
{
	UnsubscribeFromEvent(E_PHYSICSPRESTEP);

	//browser->GetHost()->CloseBrowser(true);
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	CefShutdown();
}


//
// StartCef
//
void MyApp::StartCef()
{
	CefWindowInfo window_info;
	CefBrowserSettings browserSettings;

	// browserSettings.windowless_frame_rate = 60; // 30 is default

	auto graphics = GetSubsystem<Graphics>();
	window_info.SetAsWindowless(HWND(graphics->GetExternalWindow()), false); // false means no transparency (site background colour)
	//window_info.SetAsWindowless(nullptr, false); // false means no transparency (site background colour)

	textureClient = new TextureClient(renderTextureHandler.get());

	//CefBrowserHost::CreateBrowser(window_info, textureClient.get(), "http://www.google.com", browserSettings, nullptr);
	CefBrowserHost::CreateBrowser(window_info, textureClient.get(), "http://www.radiotunes.com/chillout", browserSettings, nullptr);
	//CefBrowserHost::CreateBrowser(window_info, textureClient.get(), "https://www.youtube.com/watch?v=yYzGHhhg_og&index=171&list=PL04B59999BC5DA80D", browserSettings, nullptr);
	//browser = CefBrowserHost::CreateBrowserSync(window_info, textureClient.get(), "http://www.google.com", browserSettings, nullptr);
	//browser = CefBrowserHost::CreateBrowserSync(window_info, textureClient.get(), "https://www.youtube.com/watch?v=yYzGHhhg_og&index=171&list=PL04B59999BC5DA80D", browserSettings, nullptr);
}


//
// CreateScene
//
void MyApp::CreateScene()
{
	auto cache = GetSubsystem<ResourceCache>();

	scene = new Scene(context_);
	
	scene->CreateComponent<Octree>();
	scene->CreateComponent<PhysicsWorld>();

	// Create a scene node for the camera, which we will move around
	cameraNode = scene->CreateChild("Camera");
	cameraNode->CreateComponent<Camera>();
	cameraNode->SetPosition(Vector3(0.0f, 7.0f, -30.0f));

	// Create a zone
	auto zoneNode = scene->CreateChild("Zone");
	Zone* zone = zoneNode->CreateComponent<Zone>();
	// Set same volume as the Octree, set a close bluish fog and some ambient light
	zone->SetBoundingBox(BoundingBox(-10000.f, 10000.f));
	zone->SetAmbientColor(Color(0.05f, 0.1f, 0.15f));
	zone->SetFogColor(Color(0.1f, 0.2f, 0.3f));
	zone->SetFogStart(50.f);
	zone->SetFogEnd(2000.f);

	// Create a "screen" like object for viewing the second scene. Construct it from two StaticModels, a box for the frame
	// and a plane for the actual view
	Node* boxNode = scene->CreateChild("ScreenBox");
	boxNode->SetPosition(Vector3(0.0f, 10.0f, 0.0f));
	boxNode->SetScale(Vector3(21.0f, 16.0f, 0.5f));
	StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
	boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	boxObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	auto screenNode = scene->CreateChild("Screen");
	screenNode->SetPosition(Vector3(0.0f, 10.0f, -0.27f));
	screenNode->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
	screenNode->SetScale(Vector3(20.0f, 0.0f, 15.0f));
	auto screenObject = screenNode->CreateComponent<StaticModel>();
	screenObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

	// Create a texture and a render handler
	SharedPtr<Texture2D> texture(new Texture2D(context_));
	texture->SetSize(1024, 768, Graphics::GetRGBAFormat(), TEXTURE_RENDERTARGET);
	texture->SetFilterMode(FILTER_BILINEAR);
	renderTextureHandler = new TextureRenderHandler(texture);

	// Create a new material from scratch, use the diffuse unlit technique, assign the render texture
	// as its diffuse texture, then assign the material to the screen plane object
	SharedPtr<Material> renderMaterial(new Material(context_));
	renderMaterial->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
	renderMaterial->SetTexture(TU_DIFFUSE, renderTextureHandler->texture);
	screenObject->SetMaterial(renderMaterial);
}


//
// SetupViewport
//
void MyApp::SetupViewport()
{
	Renderer* renderer = GetSubsystem<Renderer>();

	SharedPtr<Viewport> viewport(new Viewport(context_, scene, cameraNode->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}


//
// MoveCamera
//
void MyApp::MoveCamera(float timeStep)
{
	Input* input = GetSubsystem<Input>();

	// Movement speed as world units per second
	const float MOVE_SPEED = 20.f;
	// Mouse sensitivity as degrees per pixel
	const float MOUSE_SENSITIVITY = 0.1f;

	// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
	IntVector2 mouseMove = input->GetMouseMove();
	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
	cameraNode->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

	// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
	// Use the Translate() function (default local space) to move relative to the node's orientation.
	if (input->GetKeyDown('W'))
		cameraNode->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
	if (input->GetKeyDown('S'))
		cameraNode->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
	if (input->GetKeyDown('A'))
		cameraNode->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
	if (input->GetKeyDown('D'))
		cameraNode->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}


//
// HandleKeyDown
//
void MyApp::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

	int key = eventData[P_KEY].GetInt();
	if (key == KEY_ESC)
		engine_->Exit();
}


//
// HandleUpdate
//
void MyApp::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	// Take the frame time step, which is stored as a float
	float timeStep = eventData[P_TIMESTEP].GetFloat();

	// Move the camera, scale movement with time step
	MoveCamera(timeStep);
}


//
// HandlePhysicsPrestep
//
void MyApp::HandlePhysicsPrestep(StringHash eventType, VariantMap& eventData)
{
	CefDoMessageLoopWork();
}