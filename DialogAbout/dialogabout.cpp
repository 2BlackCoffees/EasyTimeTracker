/*
 *  This file is part of EasyTimeTracker.
 *
 *  EasyTimeTracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyTimeTracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with EasyTimeTracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  EasyTimeTracker: First creation by 2BlackCoffees: http://www.twoblackcoffees.com/
 *
 **/

#include "dialogabout.h"
#include "Misc/clickableimage.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::FramelessWindowHint),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->clickableImageOK->setImagesPath(ClickableImage::mNotSelectedOKButton, ClickableImage::mSelectedOKButtonDollar);
    connect(ui->clickableImageOK, SIGNAL(clicked()), this, SLOT(accept()));
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
