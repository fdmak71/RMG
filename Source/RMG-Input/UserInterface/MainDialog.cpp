/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "MainDialog.hpp"
#include "Widget/ControllerWidget.hpp"

using namespace UserInterface;

MainDialog::MainDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    this->setupUi(this);
    this->setWindowIcon(QIcon(":Resource/RMG.png"));

    // each tab needs its own ControllerWidget
    for (int i = 0; i < this->tabWidget->count(); i++)
    {
        Widget::ControllerWidget* widget = new Widget::ControllerWidget(this);
        this->tabWidget->widget(i)->layout()->addWidget(widget);
    }
}

MainDialog::~MainDialog()
{

}
