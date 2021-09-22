/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "CustomButton.hpp"
#include "ControllerWidget.hpp"

using namespace UserInterface::Widget;

static ControllerWidget* controllerWidget = nullptr;

CustomButton::CustomButton(QWidget* parent) : QPushButton(parent)
{
	this->countDownTimer = new QTimer(this);	
    connect(this->countDownTimer, &QTimer::timeout, this, &CustomButton::on_countDownTimer_triggered);
}

CustomButton::~CustomButton()
{

}

void CustomButton::Initialize(QWidget* widget)
{
    controllerWidget = (ControllerWidget*)widget;

    connect(this, &QPushButton::released, 
    [=]
    {
        controllerWidget->on_CustomButton_released(this);
    });
}

void CustomButton::StartTimer()
{
    this->currentCountDownNum = 5;
    this->on_countDownTimer_triggered();
    this->countDownTimer->start(1000);
}

void CustomButton::StopTimer()
{
    this->countDownTimer->stop();
    controllerWidget->on_CustomButton_TimerStopped(this);
}

void CustomButton::SetButton(int button)
{
    this->button = button;
}

int CustomButton::GetButton()
{
    return this->button;
}

void CustomButton::on_countDownTimer_triggered()
{
    this->setText(QString::number(this->currentCountDownNum));

    if (this->currentCountDownNum == 0)
    {
        this->setText("");
        this->currentCountDownNum = 5;
        this->countDownTimer->stop();
        controllerWidget->on_CustomButton_TimerFinished(this);
    }

    this->currentCountDownNum--;
}
