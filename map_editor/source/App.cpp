/** \file App.cpp */
#include "App.h"

#include <string>
#include <sstream>
#include <windows.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3native.h"



// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[])
{
    //  == App Settings ==
    GApp::Settings settings(argc, argv);


    // Window caption
    settings.window.caption = argv[0];


    // Set enable to catch more OpenGL errors
    settings.window.debugContext = true;


    // Window size
    settings.window.fullScreen = false;
    settings.window.resizable = true;
    /*if (settings.window.fullScreen)
    {
        settings.window.width = 1920;
        settings.window.height = 1080;
    }
    else
    {
        settings.window.height = int(OSWindow::primaryDisplayWindowSize().y * 0.95f);
        // Constrain ultra widescreen aspect ratios
        settings.window.width = min(settings.window.height * 1920 / 1080, int(OSWindow::primaryDisplayWindowSize().x * 0.95f));

        // Make even
        settings.window.width -= settings.window.width & 1;
        settings.window.height -= settings.window.height & 1;
    }
    settings.window.resizable = !settings.window.fullScreen;
    settings.window.framed = settings.window.resizable;*/


    // Set to true for a significant performance boost if your app can't
    // render at the display frequency, or if you *want* to render faster
    // than the display.
    settings.window.asynchronous = true;


    // Render slightly larger than the screen so that screen-space refraction, bloom,
    // screen-space AO, and screen-space reflection to look good at screen edges. Set to zero for
    // maximum performance and free memory. Increase the second argument to improve AO without affecting
    // color. The third argument is the resolution. Set to 0.5f to render at half-res and upscale,
    // 2.0f to supersample.
    settings.hdrFramebuffer.setGuardBandsAndSampleRate(0, 0, 1.0f);
    settings.renderer.deferredShading = false;
    settings.renderer.orderIndependentTransparency = true;


    // Directory containing map editor-specific data
    settings.dataDir = FilePath::concat(FileSystem::currentDirectory(), "data-files");

    // Window icon
    settings.window.defaultIconFilename = "data-files/icons/icon.png";


    // Setup screen capture
    settings.screenCapture.outputDirectory = FilePath::concat(FileSystem::currentDirectory(), "../journal");
    if (! FileSystem::exists(settings.screenCapture.outputDirectory))
    {
        settings.screenCapture.outputDirectory = "";
    }
    settings.screenCapture.includeAppRevision = true;
    settings.screenCapture.includeG3DRevision = false;
    settings.screenCapture.filenamePrefix = "G2MapMaker_";


    // Window should be invisible at first
    settings.window.visible = false;

    // No window frame
    settings.window.framed = false;

    settings.window.alwaysOnTop = true;

    // Run the app
    return App(settings).run();
}


App::App(const GApp::Settings& settings) :
    GApp(settings, GLFWWindow::create(settings.window))
{

    // Declare name of mutex object for mmf access
    constexpr LPCWSTR MMF_MUTEX_NAME{ L"rpkg_G3dHwndHost_mutex" };

    // Attempt to open a mutex for accessing the memory-mapped file shared between
    // this process and the calling process
    HANDLE mmfMutex{ OpenMutex(MUTEX_ALL_ACCESS, false, MMF_MUTEX_NAME) };
    // Bail if we couldn't obtain the mutex
    assert(mmfMutex != NULL);

    // Attempt to open the memory-mapped file
    HANDLE hMmf{ OpenFileMapping(FILE_MAP_ALL_ACCESS, false, L"rpkg_G3dHwndHost_mmf") };
    // Bail if we couldn't obtain the file
    assert(hMmf != NULL);

    // Attempt to open a view of the memory-mapped file
    LPVOID mmfView{ MapViewOfFile(hMmf, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(uint8_t) + sizeof(uint64_t)) };
    // Bail if we couldn't obtain the file view
    assert(mmfView);


    // Get the app's window
    auto* appWin{ GLFWWindow::getMasterWindowPtr() };

    // Get the win32 handle of the child
    HWND childHwnd{ glfwGetWin32Window(appWin) };

    // Get the win32 handle of the parent
    HWND parentHwnd{ reinterpret_cast<HWND>(std::stoll(settings.argArray[1].c_str())) };

    // Make sure this is a child window
    SetWindowLongPtr(childHwnd, GWL_STYLE, WS_CHILD | WS_VISIBLE);
    SetParent(childHwnd, parentHwnd);

    // Make the window visible (it might already be visible since it
    //  has the WS_VISIBLE style but this is here for now just in case it isn't)
    //glfwShowWindow(appWin);
    
    // Get the parent windows position and scale
    RECT parentRect;
    assert(GetWindowRect(parentHwnd, &parentRect));
    
    // Update the child window position and scale
    
    //glfwSetWindowSize(appWin, parentRect.right - parentRect.left, parentRect.bottom - parentRect.top);
    assert(MoveWindow(
        childHwnd,
        parentRect.left,
        parentRect.top,
        parentRect.right - parentRect.left,
        parentRect.bottom - parentRect.top,
        true
    ));

    //    =Write to the mmf=
    // Set the first byte to 1 to show this process has handled the file
    uint8_t firstByte{ 1 };
    memcpy(mmfView, &firstByte, 1);
    // Add the hwnd of the child window as an unsigned 64bit integer
    auto childHwndInt{ reinterpret_cast<uint64_t>(childHwnd) };
    memcpy(
        // TODO: Do I actually need to cast it back to a void* or does the type of the pointer not matter?
        reinterpret_cast<void*>(
            reinterpret_cast<char*>(mmfView) + 1
        ),
        &childHwndInt, sizeof(uint64_t)
    );
    
    /*
    // Decalre an array of bytes to hold the data
    uint8_t mmfBytes[16] = {0};
    // Set the first byte to 1 to show this process has handled the file
    mmfBytes[0] = 1;
    // Get the hwnd of the child window as an unsigned 64bit integer
    auto childHwndInt{ reinterpret_cast<uint64_t>(childHwnd) };
    // Insert the data of that integer into the array
    for (int i{ 0 }; i < 8; ++i)
    {
        void* bytePtr{ &childHwndInt + i };
        //mmfBytes[1 + i] = reinterpret_cast<unsigned char>();
        //mmfBytes[1 + i] = *static_cast<unsigned char*>(bytePtr);
        mmfBytes[1 + i] = *reinterpret_cast<uint8_t*>(&childHwndInt + i);
    }
    memcpy(mmfView, mmfBytes, 16);
    msgBox(G3D::String{ std::to_string(childHwndInt) });
    std::string foo{ "" };
    for (auto& bar : mmfBytes)
    {
        foo += std::to_string(bar) + ", ";
    }
    msgBox(G3D::String(foo));
    */
    

    // Close the mmf and the mutex as we're done with them
    CloseHandle(hMmf);
    CloseHandle(mmfMutex);
}


G3D::String App::getUniqueEntityName(const G3D::String& name) const
{
    // Get a list of all entity names
    Array<String> names;
    m_scene->getEntityNames(names);
    
    // If the desired name isn't already taken, return it
    if (names.findIndex(name) == -1)
        return name;

    // Declare a variable to hold the number that should be
    // appended to the end of the desired name
    size_t nameSuffix{ 0 };

    // Keep trying numbers until a unique name is found
    G3D::String newName;
    do
    {
        newName = name + G3D::String(std::to_string(nameSuffix++));
    }     while (names.findIndex(newName) != -1);

    // Return the new name
    return newName;
}


// Called before the application loop begins. Load data here and
// not in the constructor so that common exceptions will be
// automatically caught.
void App::onInit()
{
    GApp::onInit();

    //setFrameDuration(1.0f / 240.0f);
    
    // Show/don't show rendering stats
    showRenderingStats = false;
    
    // Setup scene
    initScene();

    // Setup models
    ArticulatedModel::Specification spec;
    spec.filename = "models/primitive/cube.obj";
    //spec.stripMaterials = true;
    m_scene->createModel(spec.toAny(), "cube");

    // Make the GUI after the scene is loaded because loading/rendering/simulation initialize
    // some variables that advanced GUIs may wish to reference with pointers.
    makeGUI();
}


// TODO: Shouldn't be able to drag toolbar
void App::makeGUI()
{
    return;
    // Load icons
    // Using the Tango Icon Library: https://en.wikipedia.org/wiki/Tango_Desktop_Project
    shared_ptr<IconSet> icons = IconSet::fromFile(System::findDataFile("tango.icn"));

    //  HEADER
    const shared_ptr<GuiWindow>& guiHeader{ GuiWindow::create(
        // Title
        "header",
        // Theme
        shared_ptr<GuiTheme>(),
        // Position & Size
        Rect2D::xywh(0.0f, 0.0f, static_cast<float>(window()->width()), 50.0f),
        // Style
        GuiTheme::PANEL_WINDOW_STYLE
    ) };
    GuiPane* guiHeaderPane{ guiHeader->pane() };

    guiHeaderPane->addButton("File", GuiTheme::ButtonStyle::NORMAL_BUTTON_STYLE);
    
    addWidget(guiHeader);
    //m_fullwidthGuiCollection["header"] = std::dynamic_pointer_cast<GuiControl>(guiHeader);
    m_fullwidthGuiCollection["header"] = guiHeader.get();


    //  TOOLBAR
    const shared_ptr<GuiWindow>& guiToolbar{ GuiWindow::create(
        // Title
        "toolbar",
        // Theme
        shared_ptr<GuiTheme>(),
        // Position & Size
        Rect2D::xywh(0.0f, 50.0f, static_cast<float>(window()->width()), 50.0f),
        // Style
        GuiTheme::TOOL_WINDOW_STYLE
    ) };
    GuiPane* guiToolbarPane{ guiToolbar->pane() };

    guiToolbarPane->addButton(icons->get("22x22/uwe/CreateBox.png"), [this]() { this->onClick_AddCube(); }, GuiTheme::TOOL_BUTTON_STYLE);

    addWidget(guiToolbar);
    m_fullwidthGuiCollection["toolbar"] = guiToolbar.get();
}


void App::initScene()
{
    // Load an empty scene from a file
    loadScene("data-files/scenes/empty.Scene.Any");
    // Get a pointer to the loaded scene
    m_scene = scene().get();

    // Allow the scene to be edited
    m_scene->setEditing(true);
}


void App::onClick_AddCube()
{
    // Create a new entity
    shared_ptr<Entity> newEnt{ VisibleEntity::create(
        // Entity name
        getUniqueEntityName("cube"),
        // Scene entity is in
        m_scene,
        // Model to use
        m_scene->modelTable()["cube"].resolve(),
        // Transform
        CFrame::fromXYZYPRDegrees(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
        ) };

    // Insert the new entity into the scene hierarchy
    m_scene->insert(newEnt);
}


void App::onAI()
{
    GApp::onAI();
    // Add non-simulation game logic and AI code here
}


void App::onNetwork()
{
    GApp::onNetwork();
    // Poll net messages here
}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt)
{
    GApp::onSimulation(rdt, sdt, idt);

    // Example GUI dynamic layout code.  Resize the debugWindow to fill
    // the screen horizontally.
    //debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


bool App::onEvent(const GEvent& event)
{
    // Handle super-class events
    if (GApp::onEvent(event)) { return true; }

    switch (event.type)
    {
    case GEventType::VIDEO_RESIZE:
        onResizeWindow(event);
    }

    // If you need to track individual UI events, manage them here.
    // Return true if you want to prevent other parts of the system
    // from observing this specific event.
    //
    // For example,
    // if ((event.type == GEventType::GUI_ACTION) && (event.gui.control == m_button)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == GKey::TAB)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == 'p')) { ... return true; }

    return false;
}


void App::onResizeWindow(const GEvent& event)
{
    return;
    // Update gui layout
    // TODO: Don't manually resize everything
    for (auto& controlPair : m_fullwidthGuiCollection)
    {
        auto* control{ controlPair.second };
        if (controlPair.second)
        {
            const Rect2D controlRect{ control->rect() };

            control->setRect(Rect2D::xywh(
                controlRect.x0(),
                controlRect.y0(),
                static_cast<float>(control->window()->width()),
                controlRect.height()
            ));
        }
    }
}


void App::onUserInput(UserInput* ui)
{
    GApp::onUserInput(ui);
    (void)ui;
    // Add key handling here based on the keys currently held or
    // ones that changed in the last frame.
}


void App::onPose(Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D)
{
    GApp::onPose(surface, surface2D);

    // Append any models to the arrays that you want to later be rendered by onGraphics()
}


void App::onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& posed2D)
{
    // Render 2D objects like Widgets.  These do not receive tone mapping or gamma correction.
    Surface2D::sortAndRender(rd, posed2D);
}


void App::onCleanup()
{
    // Called after the application loop ends.  Place a majority of cleanup code
    // here instead of in the constructor so that exceptions can be caught.
}
