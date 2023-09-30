#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <QWidget>
#include <QSurfaceFormat>
#include <QWindow>
#include <gui/state.h>

namespace Ui {
class RenderWindow;
}

class RenderWindow : public QWidget {
    Q_OBJECT

public:
    explicit RenderWindow(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);

    QPaintEngine* paintEngine() const override {
        return nullptr;
    }

    bool init_render_target();

private:
    GuiState &gui;
    EmuEnvState &emuenv;

    QWidget *child_widget = nullptr;

    bool initialize_gl();
    bool initialize_vulkan();
};

class RenderWidget : public QWidget {
    Q_OBJECT

public:
    explicit RenderWidget(RenderWindow* parent) : QWidget(parent), render_window(parent) {
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
    }

private:
    RenderWindow *render_window;
};

class OpenGLRenderWidget : public RenderWidget {
public:
    explicit OpenGLRenderWidget(RenderWindow *parent) : RenderWidget(parent) {
        windowHandle()->setSurfaceType(QWindow::OpenGLSurface);
    }
};

class VulkanRenderWidget : public RenderWidget {
public:
    explicit VulkanRenderWidget(RenderWindow *parent) : RenderWidget(parent) {
        windowHandle()->setSurfaceType(QWindow::VulkanSurface);
    }
};

#endif // RENDER_WINDOW_H
