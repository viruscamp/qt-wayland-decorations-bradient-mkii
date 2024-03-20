// Copyright (C) 2016 Robin Burchell <robin.burchell@viroteck.net>
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPalette>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainterPath>

#include <qpa/qwindowsysteminterface.h>
#include <QtGui/private/qhighdpiscaling_p.h>

#include <QtWaylandClient/private/qwaylanddecorationplugin_p.h>
#include <QtWaylandClient/private/qwaylandabstractdecoration_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

QT_BEGIN_NAMESPACE

namespace QtWaylandClient {

#define BUTTON_SPACING 5
#define BUTTON_WIDTH 18
#define BUTTONS_RIGHT_MARGIN 8
#define BUTTON_PEN_WIDTH_THICK 2
#define BUTTON_PEN_WIDTH_THIN 1
#define BUTTON_MAXIMIZE_PADDING_HORIZON 4
#define BUTTON_MAXIMIZE_PADDING_VERTICAL 5
#define BUTTON_MINIMIZE_PADDING 5
#define ICON_WIDTH 22
#define ICON_PADDING 4
#define TITLE_BAR_HEIGHT 30
#define TITLE_BAR_RADIUS 3
#define FONT_SIZE_PX 14

enum Button
{
    None,
    Close,
    Maximize,
    Minimize
};

class Q_WAYLANDCLIENT_EXPORT QWaylandBradientMkiiDecoration : public QWaylandAbstractDecoration
{
public:
    QWaylandBradientMkiiDecoration();
protected:
    QMargins margins(MarginsType marginsType = Full) const override;
    void paint(QPaintDevice *device) override;
    bool handleMouse(QWaylandInputDevice *inputDevice, const QPointF &local, const QPointF &global,Qt::MouseButtons b,Qt::KeyboardModifiers mods) override;
    bool handleTouch(QWaylandInputDevice *inputDevice, const QPointF &local, const QPointF &global, QEventPoint::State state, Qt::KeyboardModifiers mods) override;
private:
    enum class PointerType {
        Mouse,
        Touch
    };

    int scale(int x) const;

    void processPointerTop(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods, PointerType type);
    void processPointerBottom(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods, PointerType type);
    void processPointerLeft(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods, PointerType type);
    void processPointerRight(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods, PointerType type);
    bool clickButton(Qt::MouseButtons b, Button btn);

    QRectF closeButtonRect() const;
    QRectF maximizeButtonRect() const;
    QRectF minimizeButtonRect() const;

    QStaticText m_windowTitle;
    Button m_clicking = None;
};



QWaylandBradientMkiiDecoration::QWaylandBradientMkiiDecoration()
{
    QTextOption option(Qt::AlignHCenter | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::NoWrap);
    m_windowTitle.setTextOption(option);
    m_windowTitle.setTextFormat(Qt::PlainText);
}

int QWaylandBradientMkiiDecoration::scale(int x) const
{
    return qFloor(x * QHighDpiScaling::factor(waylandWindow()->screen()));
}

QRectF QWaylandBradientMkiiDecoration::closeButtonRect() const
{
    const int windowRight = waylandWindow()->windowContentGeometry().right() + 1;
    int button_width = scale(BUTTON_WIDTH);
    return QRectF(windowRight - button_width - scale(BUTTON_SPACING) * 0 - scale(BUTTONS_RIGHT_MARGIN),
                  (margins().top() - button_width) / 2, button_width, button_width);
}

QRectF QWaylandBradientMkiiDecoration::maximizeButtonRect() const
{
    const int windowRight = waylandWindow()->windowContentGeometry().right() + 1;
    int button_width = scale(BUTTON_WIDTH);
    return QRectF(windowRight - button_width * 2 - scale(BUTTON_SPACING) * 1 - scale(BUTTONS_RIGHT_MARGIN),
                  (margins().top() - button_width) / 2, button_width, button_width);
}

QRectF QWaylandBradientMkiiDecoration::minimizeButtonRect() const
{
    const int windowRight = waylandWindow()->windowContentGeometry().right() + 1;
    int button_width = scale(BUTTON_WIDTH);
    return QRectF(windowRight - button_width * 3 - scale(BUTTON_SPACING) * 2 - scale(BUTTONS_RIGHT_MARGIN),
                  (margins().top() - button_width) / 2, button_width, button_width);
}

QMargins QWaylandBradientMkiiDecoration::margins(MarginsType marginsType) const
{
    if (marginsType == ShadowsOnly)
        return QMargins();

    return QMargins(scale(TITLE_BAR_RADIUS), scale(TITLE_BAR_HEIGHT), scale(TITLE_BAR_RADIUS), scale(TITLE_BAR_RADIUS));
}

void QWaylandBradientMkiiDecoration::paint(QPaintDevice *device)
{
    bool active = window()->handle()->isActive();
    QRect wg = waylandWindow()->windowContentGeometry();
    QRect clips[] =
    {
        QRect(wg.left(), wg.top(), wg.width(), margins().top()),
        QRect(wg.left(), (wg.bottom() + 1) - margins().bottom(), wg.width(), margins().bottom()),
        QRect(wg.left(), margins().top(), margins().left(), wg.height() - margins().top() - margins().bottom()),
        QRect((wg.right() + 1) - margins().right(), wg.top() + margins().top(), margins().right(), wg.height() - margins().top() - margins().bottom())
    };

    QRect top = clips[0];

    QPalette palette;
    const QColor foregroundColor = palette.color(QPalette::Active, QPalette::WindowText);
    const QColor backgroundColor = palette.color(QPalette::Active, QPalette::Window);
    const QColor foregroundInactiveColor = palette.color(QPalette::Disabled, QPalette::WindowText);

    QPainter p(device);
    p.setRenderHint(QPainter::Antialiasing);

    // Title bar
    QPainterPath roundedRect;
    roundedRect.addRoundedRect(wg, scale(TITLE_BAR_RADIUS), scale(TITLE_BAR_RADIUS));
    for (int i = 0; i < 4; ++i) {
        p.save();
        p.setClipRect(clips[i]);
        p.fillPath(roundedRect, backgroundColor);
        p.restore();
    }

    int button_spacing = scale(BUTTON_SPACING);
    int icon_width = scale(ICON_WIDTH);
    int icon_padding = scale(ICON_PADDING);
    // Window icon
    QIcon icon = waylandWindow()->windowIcon();
    if (!icon.isNull()) {
        QRectF iconRect(0, 0, icon_width, icon_width);
        iconRect.adjust(margins().left() + button_spacing, icon_padding,
                        margins().left() + button_spacing, icon_padding),
        icon.paint(&p, iconRect.toRect());
    }

    // Window title
    QString windowTitleText = window()->title();
    if (!windowTitleText.isEmpty()) {
        QFont font = p.font();
        font.setPixelSize(scale(FONT_SIZE_PX));

        if (m_windowTitle.text() != windowTitleText) {
            m_windowTitle.setText(windowTitleText);
            m_windowTitle.prepare(QTransform(), font);
        }

        QRect titleBar = top;
        titleBar.setLeft(margins().left() + button_spacing +
            (icon.isNull() ? 0 : icon_width + button_spacing));
        titleBar.setRight(minimizeButtonRect().left() - button_spacing);

        p.save();
        p.setClipRect(titleBar);
        p.setPen(active ? foregroundColor : foregroundInactiveColor);
        QSizeF size = m_windowTitle.size();
        int dx = (top.width() - size.width()) /2;
        int dy = (top.height()- size.height()) /2;
        p.setFont(font);
        QPoint windowTitlePoint(top.topLeft().x() + dx,
                 top.topLeft().y() + dy);
        p.drawStaticText(windowTitlePoint, m_windowTitle);
        p.restore();
    }

    QRectF rect;

    // Default pen
    QPen pen(active ? foregroundColor : foregroundInactiveColor);
    p.setPen(pen);
    int pen_width_thick = scale(BUTTON_PEN_WIDTH_THICK);
    int pen_width_thin = scale(BUTTON_PEN_WIDTH_THIN);

    // Close button
    p.save();
    rect = closeButtonRect();
    qreal crossSize = rect.height() / 2.3;
    QPointF crossCenter(rect.center());
    QRectF crossRect(crossCenter.x() - crossSize / 2, crossCenter.y() - crossSize / 2, crossSize, crossSize);
    pen.setWidth(pen_width_thick);
    p.setPen(pen);
    p.drawLine(crossRect.topLeft(), crossRect.bottomRight());
    p.drawLine(crossRect.bottomLeft(), crossRect.topRight());
    p.restore();

    // Maximize button
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    pen.setWidth(pen_width_thin);
    p.setPen(pen);
    int mph = scale(BUTTON_MAXIMIZE_PADDING_HORIZON);
    int mpv = scale(BUTTON_MAXIMIZE_PADDING_VERTICAL);
    rect = maximizeButtonRect().adjusted(mph, mpv, -mph, -mpv);
    if ((window()->windowStates() & Qt::WindowMaximized)) {
        qreal inset = pen_width_thick;
        QRectF rect1 = rect.adjusted(inset, 0, 0, -inset);
        QRectF rect2 = rect.adjusted(0, inset, -inset, 0);
        p.drawRect(rect1);
        p.setBrush(backgroundColor); // need to cover up some lines from the other rect
        p.drawRect(rect2);
    } else {
        p.drawRect(rect);
        p.drawLine(rect.left(), rect.top() + pen_width_thin, rect.right(), rect.top() + pen_width_thin);
    }
    p.restore();

    // Minimize button
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    int mp = scale(BUTTON_MINIMIZE_PADDING);
    rect = minimizeButtonRect().adjusted(mp, mp, -mp, -mp);
    pen.setWidth(pen_width_thick);
    p.setPen(pen);
    p.drawLine(rect.bottomLeft(), rect.bottomRight());
    p.restore();
}

bool QWaylandBradientMkiiDecoration::clickButton(Qt::MouseButtons b, Button btn)
{
    if (isLeftClicked(b)) {
        m_clicking = btn;
        return false;
    } else if (isLeftReleased(b)) {
        if (m_clicking == btn) {
            m_clicking = None;
            return true;
        } else {
            m_clicking = None;
        }
    }
    return false;
}

bool QWaylandBradientMkiiDecoration::handleMouse(QWaylandInputDevice *inputDevice, const QPointF &local, const QPointF &global, Qt::MouseButtons b, Qt::KeyboardModifiers mods)
{
    Q_UNUSED(global);

    // Figure out what area mouse is in
    QRect wg = waylandWindow()->windowContentGeometry();
    if (local.y() <= wg.top() + margins().top()) {
        processPointerTop(inputDevice, local, b, mods, PointerType::Mouse);
    } else if (local.y() > wg.bottom() - margins().bottom()) {
        processPointerBottom(inputDevice, local, b, mods, PointerType::Mouse);
    } else if (local.x() <= wg.left() + margins().left()) {
        processPointerLeft(inputDevice, local, b, mods, PointerType::Mouse);
    } else if (local.x() > wg.right() - margins().right()) {
        processPointerRight(inputDevice, local, b, mods, PointerType::Mouse);
    } else {
#if QT_CONFIG(cursor)
        waylandWindow()->restoreMouseCursor(inputDevice);
#endif
        setMouseButtons(b);
        return false;
    }

    setMouseButtons(b);
    return true;
}

bool QWaylandBradientMkiiDecoration::handleTouch(QWaylandInputDevice *inputDevice, const QPointF &local, const QPointF &global, QEventPoint::State state, Qt::KeyboardModifiers mods)
{
    Q_UNUSED(global);
    QRect wg = waylandWindow()->windowContentGeometry();

    bool handled = state == QEventPoint::Pressed;
    if (handled) {
        if (local.y() <= wg.top() + margins().top()) {
            processPointerTop(inputDevice, local, Qt::LeftButton, mods, PointerType::Touch);
        } else if (local.y() > wg.bottom() - margins().bottom()) {
            processPointerBottom(inputDevice, local, Qt::LeftButton, mods, PointerType::Touch);
        } else if (local.x() <= wg.left() + margins().left()) {
            processPointerLeft(inputDevice, local, Qt::LeftButton, mods, PointerType::Touch);
        } else if (local.x() > wg.right() - margins().right()) {
            processPointerRight(inputDevice, local, Qt::LeftButton, mods, PointerType::Touch);
        } else {
            handled = false;
        }
    }

    return handled;
}

void QWaylandBradientMkiiDecoration::processPointerTop(QWaylandInputDevice *inputDevice,
                                                 const QPointF &local,
                                                 Qt::MouseButtons b,
                                                 Qt::KeyboardModifiers mods,
                                                 PointerType type)
{
#if !QT_CONFIG(cursor)
    Q_UNUSED(type);
#endif

    QRect wg = waylandWindow()->windowContentGeometry();
    Q_UNUSED(mods);
    if (local.y() <= wg.top() + margins().bottom()) {
        if (local.x() <= margins().left()) {
            //top left bit
#if QT_CONFIG(cursor)
            if (type == PointerType::Mouse)
                waylandWindow()->setMouseCursor(inputDevice, Qt::SizeFDiagCursor);
#endif
            startResize(inputDevice, Qt::TopEdge | Qt::LeftEdge, b);
        } else if (local.x() > wg.right() - margins().right()) {
            //top right bit
#if QT_CONFIG(cursor)
            if (type == PointerType::Mouse)
                waylandWindow()->setMouseCursor(inputDevice, Qt::SizeBDiagCursor);
#endif
            startResize(inputDevice, Qt::TopEdge | Qt::RightEdge, b);
        } else {
            //top resize bit
#if QT_CONFIG(cursor)
            if (type == PointerType::Mouse)
                waylandWindow()->setMouseCursor(inputDevice, Qt::SplitVCursor);
#endif
            startResize(inputDevice, Qt::TopEdge, b);
        }
    } else if (local.x() <= wg.left() + margins().left()) {
        processPointerLeft(inputDevice, local, b, mods, type);
    } else if (local.x() > wg.right() - margins().right()) {
        processPointerRight(inputDevice, local, b, mods, type);
    } else if (isRightClicked(b)) {
        showWindowMenu(inputDevice);
    } else if (closeButtonRect().contains(local)) {
        if (type == PointerType::Touch || clickButton(b, Close))
            QWindowSystemInterface::handleCloseEvent(window());
    } else if (maximizeButtonRect().contains(local)) {
        if (type == PointerType::Touch || clickButton(b, Maximize))
            window()->setWindowStates(window()->windowStates() ^ Qt::WindowMaximized);
    } else if (minimizeButtonRect().contains(local)) {
        if (type == PointerType::Touch || clickButton(b, Minimize))
            window()->setWindowState(Qt::WindowMinimized);
    } else {
#if QT_CONFIG(cursor)
        if (type == PointerType::Mouse)
            waylandWindow()->restoreMouseCursor(inputDevice);
#endif
        startMove(inputDevice,b);
    }
}

void QWaylandBradientMkiiDecoration::processPointerBottom(QWaylandInputDevice *inputDevice,
                                                      const QPointF &local,
                                                      Qt::MouseButtons b,
                                                      Qt::KeyboardModifiers mods,
                                                      PointerType type)
{
    Q_UNUSED(mods);
#if !QT_CONFIG(cursor)
    Q_UNUSED(type);
#endif

    if (local.x() <= margins().left()) {
        //bottom left bit
#if QT_CONFIG(cursor)
        if (type == PointerType::Mouse)
            waylandWindow()->setMouseCursor(inputDevice, Qt::SizeBDiagCursor);
#endif
        startResize(inputDevice, Qt::BottomEdge | Qt::LeftEdge, b);
    } else if (local.x() > window()->width() + margins().left()) {
        //bottom right bit
#if QT_CONFIG(cursor)
        if (type == PointerType::Mouse)
            waylandWindow()->setMouseCursor(inputDevice, Qt::SizeFDiagCursor);
#endif
        startResize(inputDevice, Qt::BottomEdge | Qt::RightEdge, b);
    } else {
        //bottom bit
#if QT_CONFIG(cursor)
        if (type == PointerType::Mouse)
            waylandWindow()->setMouseCursor(inputDevice, Qt::SizeVerCursor);
#endif
        startResize(inputDevice, Qt::BottomEdge, b);
    }
}

void QWaylandBradientMkiiDecoration::processPointerLeft(QWaylandInputDevice *inputDevice,
                                                    const QPointF &local,
                                                    Qt::MouseButtons b,
                                                    Qt::KeyboardModifiers mods,
                                                    PointerType type)
{
    Q_UNUSED(local);
    Q_UNUSED(mods);
#if QT_CONFIG(cursor)
    if (type == PointerType::Mouse)
        waylandWindow()->setMouseCursor(inputDevice, Qt::SizeHorCursor);
#else
    Q_UNUSED(type);
#endif
    startResize(inputDevice, Qt::LeftEdge, b);
}

void QWaylandBradientMkiiDecoration::processPointerRight(QWaylandInputDevice *inputDevice,
                                                     const QPointF &local,
                                                     Qt::MouseButtons b,
                                                     Qt::KeyboardModifiers mods,
                                                     PointerType type)
{
    Q_UNUSED(local);
    Q_UNUSED(mods);
#if QT_CONFIG(cursor)
    if (type == PointerType::Mouse)
        waylandWindow()->setMouseCursor(inputDevice, Qt::SizeHorCursor);
#else
    Q_UNUSED(type);
#endif
    startResize(inputDevice, Qt::RightEdge, b);
}

class QWaylandBradientMkiiDecorationPlugin : public QWaylandDecorationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWaylandDecorationFactoryInterface_iid FILE "bradient-mkii.json")
public:
    QWaylandAbstractDecoration *create(const QString&, const QStringList&) override;
};

QWaylandAbstractDecoration *QWaylandBradientMkiiDecorationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    Q_UNUSED(system);
    return new QWaylandBradientMkiiDecoration();
}

}

QT_END_NAMESPACE

#include "main.moc"
