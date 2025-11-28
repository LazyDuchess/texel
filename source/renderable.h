#pragma once
#include "component.h"
#include "platform/renderer.h"

class Renderable : public Component {
public:
    virtual void QueueRenderCommands() = 0;
    virtual void OnAdd();
    virtual void OnRemove();
};