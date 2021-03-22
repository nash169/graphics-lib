#include "magnum_dynamics/MagnumApp.hpp"

namespace magnum_dynamics {
    void MagnumApp::viewportEvent(ViewportEvent& event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        _camera->setViewport(event.windowSize());
    }

    void MagnumApp::mousePressEvent(MouseEvent& event)
    {
        if (event.button() == MouseEvent::Button::Left)
            _previousPosition = positionOnSphere(event.position());
    }

    void MagnumApp::mouseReleaseEvent(MouseEvent& event)
    {
        if (event.button() == MouseEvent::Button::Left)
            _previousPosition = Vector3();
    }

    void MagnumApp::mouseScrollEvent(MouseScrollEvent& event)
    {
        if (!event.offset().y())
            return;

        /* Distance to origin */
        const Float distance = (*_cameraObject).transformation().translation().z();

        /* Move 15% of the distance back or forward */
        (*_cameraObject).translate(Vector3::zAxis(distance * (1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f))));

        redraw();
    }

    void MagnumApp::mouseMoveEvent(MouseMoveEvent& event)
    {
        if (!(event.buttons() & MouseMoveEvent::Button::Left))
            return;

        const Vector3 currentPosition = positionOnSphere(event.position());
        const Vector3 axis = Math::cross(_previousPosition, currentPosition);

        if (_previousPosition.length() < 0.001f || axis.length() < 0.001f)
            return;

        _manipulator.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
        _previousPosition = currentPosition;

        redraw();
    }

    Vector3 MagnumApp::positionOnSphere(const Vector2i& position) const
    {
        const Vector2 positionNormalized = Vector2{position} / Vector2{_camera->viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
        return (result * Vector3::yScale(-1.0f)).normalized();
    }
} // namespace magnum_dynamics