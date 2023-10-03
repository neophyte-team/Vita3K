#include "render_window.h"
#include "QHBoxLayout"
#include <emuenv/state.h>
#include <renderer/state.h>

RenderWindow::RenderWindow(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWidget(parent),
    gui{gui_},
    emuenv{emuenv_} {

    setWindowTitle(QStringLiteral("Vita3K"));
    setAttribute(Qt::WA_AcceptTouchEvents);
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

bool RenderWindow::init_render_target() {
    switch (emuenv.backend_renderer) {
    case renderer::Backend::OpenGL:
        initialize_gl();
        break;

    case renderer::Backend::Vulkan:
        initialize_vulkan();
        break;
    }
    layout()->addWidget(child_widget);
    return true;
}

bool RenderWindow::initialize_gl() {
    auto child = new OpenGLRenderWidget(this);
    child_widget = child;
    child_widget->windowHandle()->create();
    return true;
}

bool RenderWindow::initialize_vulkan() {
    auto child = new VulkanRenderWidget(this);
    child_widget = child;
    child_widget->windowHandle()->create();
    return true;
}
