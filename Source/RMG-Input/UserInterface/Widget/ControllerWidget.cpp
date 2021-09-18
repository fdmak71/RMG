/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "ControllerWidget.hpp"

#include <QPixmap>
#include <QResizeEvent>
#include <qcolor.h>
#include <qnamespace.h>

using namespace UserInterface::Widget;

ControllerWidget::ControllerWidget(QWidget* parent) : QWidget(parent)
{
	this->setupUi(this);

    int w = this->size().width();
    int h = this->size().height();


    QPixmap image = QIcon(":Resource/Controller.svg").pixmap(QSize(w, h));

    this->imageLabel->setPixmap(image);
    this->imageLabel->setMinimumSize(QSize(w, h));
}

ControllerWidget::~ControllerWidget()
{

}

#include <iostream>

void ControllerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int width = event->size().width() - event->oldSize().width();
    int height = event->size().height() - event->oldSize().height();

    QSize oldImageLabelSize = this->imageLabel->size();
    QSize newImageLabelSize = QSize(oldImageLabelSize.width() + width, oldImageLabelSize.height() + height);

    std::cout << "oldSize " << event->oldSize().width() << "x" << event->oldSize().height() << std::endl;
    std::cout << "newSize " << event->size().width() << "x" << event->size().height() << std::endl;
    std::cout << "oldImageLabelSize " << oldImageLabelSize.width() << "x" << oldImageLabelSize.height() << std::endl;
    std::cout << "newImageLabelSize " << newImageLabelSize.width() << "x" << newImageLabelSize.height() << std::endl;
}
