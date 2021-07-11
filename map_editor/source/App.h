/**
  \file App.h

  The G3D 10.00 default starter app is configured for OpenGL 4.1 and
  relatively recent GPUs.
 */
#pragma once
#include <G3D/G3D.h>
#include <string>
#include <string_view>
#include <unordered_map>

/**
 * @brief Application framework
 * @author Andrew Pratt
 */
class App : public GApp {
private:
    /** Raw pointer to the current scene for convenience */
    Scene* m_scene;
    /**
     * @brief Collection of all gui elements who's width should span it's parent
     * 
     * TODO: This shouldn't be needed. Also the raw pointer will not work if any addresses move
     */
    std::unordered_map<std::string_view, GuiWindow*> m_fullwidthGuiCollection;

protected:

    /** Called from onInit */
    void makeGUI();


    /** @brief Initializes an empty scene */
    void initScene();


    /** @brief Click handler for add cube button */
    void onClick_AddCube();

    /** @brief Handler for when window is resized */
    void onResizeWindow(const GEvent& event);


public:
    // Constructor
    App(const GApp::Settings& settings = GApp::Settings());

    /**
     * @brief Get a unique version of a desired entity name, if it isn't already unique
     * 
     * @param name Desired entity name
     * @return Desired name of the entity, which may be modified to ensure it's unique
     * @warning Assumes m_scene has been initialized and is valid
     * 
     * @throw std::overflow_error If too many entities have the same desired name, probably will never happen unless you have
     *      over 18,446,744,073,709,551,615 entities with the same name
     */
    G3D::String getUniqueEntityName(const G3D::String& name) const;

    // Method overrides
#pragma region overrides
    virtual void onInit() override;
    virtual void onAI() override;
    virtual void onNetwork() override;
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;
    virtual void onPose(Array<shared_ptr<Surface> >& posed3D, Array<shared_ptr<Surface2D> >& posed2D) override;

    // You can override onGraphics if you want more control over the rendering loop.
    // virtual void onGraphics(RenderDevice* rd, Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) override;

    //virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
    virtual void onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual bool onEvent(const GEvent& e) override;
    virtual void onUserInput(UserInput* ui) override;
    virtual void onCleanup() override;
#pragma endregion
};
