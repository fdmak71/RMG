/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CUSTOMBUTTON_HPP
#define CUSTOMBUTTON_HPP

#include <QPushButton>
#include <QTimer>

namespace UserInterface
{
namespace Widget
{

// we can't include ControllerWidget.hpp
// because that header includes this header,
// so forward-declare it to prevent issues
class ControllerWidget;

class CustomButton : public QPushButton
{
private:
	QTimer* countDownTimer = nullptr;
	ControllerWidget* controllerWidget = nullptr;
	int currentCountDownNum = 0;
	int button = -1;

public:
	CustomButton(QWidget* parent);
	~CustomButton();
	void Initialize(QWidget* widget);

	void StartTimer();
	void StopTimer();

	void SetButton(int button);
	int GetButton();

private slots:
	void on_countDownTimer_triggered();
};
}
}


#endif // CUSTOMBUTTON_HPP
