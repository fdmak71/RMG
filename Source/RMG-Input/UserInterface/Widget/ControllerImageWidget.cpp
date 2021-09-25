/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "ControllerImageWidget.hpp"

#include <QSvgRenderer>
#include <cmath>
#include <iostream>
#include <QPainter>
#include <QResizeEvent>

using namespace UserInterface::Widget;

ControllerImageWidget::ControllerImageWidget(QWidget* parent) : QWidget(parent)
{
}

ControllerImageWidget::~ControllerImageWidget()
{

}

void ControllerImageWidget::SetButtonState(enum N64CONTROLLER_BUTTON button, bool state)
{
    if (this->buttonState[button] != state)
    {
        this->buttonState[button] = state;
        this->needImageUpdate = true;
    }
}

void ControllerImageWidget::SetXAxisState(int xAxis)
{
    if (this->xAxisState != xAxis)
    {
        this->xAxisState = xAxis;
        this->needImageUpdate = true;
    }
}

void ControllerImageWidget::SetYAxisState(int yAxis)
{
    if (this->yAxisState != yAxis)
    {
        this->yAxisState = yAxis;
        this->needImageUpdate = true;
    }
}

void ControllerImageWidget::SetDeadzone(int value)
{
    if (this->deadzoneValue != value)
    {
        this->deadzoneValue = value;
        this->needImageUpdate = true;
    }
}

void ControllerImageWidget::ClearControllerState()
{
    // reset button state
    for (int i = 0; i < N64CONTROLLER_NUM; i++)
    {
        if (this->buttonState[i])
        {
            this->buttonState[i] = false;
            this->needImageUpdate = true;
        }
    }

    // reset x axis state
    if (this->xAxisState != 0)
    {
        this->xAxisState = 0;
        this->needImageUpdate = true;
    }

    // reset y axis state
    if (this->yAxisState != 0)
    {
        this->yAxisState = 0;
        this->needImageUpdate = true;
    }

    this->UpdateImage();
}

void ControllerImageWidget::UpdateImage()
{
    if (this->needImageUpdate)
    {
        this->update();
        this->needImageUpdate = false;
    }
}


void ControllerImageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QSvgRenderer renderer;
 
    static const struct
    {
        enum N64CONTROLLER_BUTTON button;
        QString imageUri;
    } buttons[] =
    {
        { N64CONTROLLER_BUTTON_A, ":Resource/Controller_Pressed_A.svg" },
        { N64CONTROLLER_BUTTON_B, ":Resource/Controller_Pressed_B.svg" },
        { N64CONTROLLER_BUTTON_START, ":Resource/Controller_Pressed_Start.svg" },
        { N64CONTROLLER_BUTTON_DPAD_UP, ":Resource/Controller_Pressed_DpadUp.svg" },
        { N64CONTROLLER_BUTTON_DPAD_DOWN, ":Resource/Controller_Pressed_DpadDown.svg" },
        { N64CONTROLLER_BUTTON_DPAD_LEFT, ":Resource/Controller_Pressed_DpadLeft.svg" },
        { N64CONTROLLER_BUTTON_DPAD_RIGHT, ":Resource/Controller_Pressed_DpadRight.svg" },
        { N64CONTROLLER_BUTTON_CBUTTONS_UP, ":Resource/Controller_Pressed_CButtonUp.svg" },
        { N64CONTROLLER_BUTTON_CBUTTONS_DOWN, ":Resource/Controller_Pressed_CButtonDown.svg" },
        { N64CONTROLLER_BUTTON_CBUTTONS_LEFT, ":Resource/Controller_Pressed_CButtonLeft.svg" },
        { N64CONTROLLER_BUTTON_CBUTTONS_RIGHT, ":Resource/Controller_Pressed_CButtonRight.svg" },
        { N64CONTROLLER_BUTTON_LEFTTRIGGER, ":Resource/Controller_Pressed_LeftTrigger.svg" },
        { N64CONTROLLER_BUTTON_RIGHTTRIGGER, ":Resource/Controller_Pressed_RightTrigger.svg" },
    };

    static const QString baseImageUri = ":Resource/Controller_NoAnalogStick.svg";
    static const QString analogStickImageUri = ":Resource/Controller_AnalogStick.svg";

    // render base image first
    renderer.load(baseImageUri);
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    renderer.render(&painter);

    // render button images on top
    // when the button is pressed
    for (auto& button : buttons)
    {
        if (this->buttonState[button.button])
        {
            renderer.load(button.imageUri);
            renderer.setAspectRatioMode(Qt::KeepAspectRatio);
            renderer.render(&painter);
        }
    }

    // render analog stick
    renderer.load(analogStickImageUri);
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    QRectF rectF = renderer.viewBoxF();
    double offsetx = 0, offsety = 0;
    static const int width = rectF.width();
    static const int height = rectF.height();
    // we'll move the analog stick by a percentage
    // of the total width/height from the image
    static const double maxOffsety = ((double)(height * 0.12265f) / 2);
    static const double maxOffsetx = maxOffsety;//(double)height * 0.12265f;

    // take deadzone into account
    if (sqrt(pow(this->xAxisState, 2) + pow(this->yAxisState, 2)) > this->deadzoneValue)
    {
        offsetx = -(maxOffsetx / 100 * this->xAxisState);
        offsety = -(maxOffsety / 100 * this->yAxisState);
    }

    // adjust rect with offset
    rectF.adjust(
        // dx1
        offsetx,
        // dy1
        offsety,
        // dx2
        0,
        // dy2
        0
    );

    // make sure we keep the same 
    // width and height
    rectF.setWidth(width);
    rectF.setHeight(height);

    // set viewbox and render
    renderer.setViewBox(rectF);
    renderer.render(&painter);
}

void ControllerImageWidget::resizeEvent(QResizeEvent *event)
{
    const QSize& currentSize = event->size();

    // calculate expected width & height
    const int width = currentSize.width();
    const int height = std::ceil(currentSize.width() * 0.9759f);
    const QSize expectedSize = QSize(width, height);

    // set widget's size as needed,
    // the widget resize function doesn't work
    // as expected, so set the minimum size
    if (currentSize != expectedSize)
    {
        this->setMinimumSize(expectedSize);
        this->update();
    }

    QWidget::resizeEvent(event);
}
