#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget * parent) :
     QLabel(parent)
{

}

ClickableLabel::ClickableLabel(const QString & text, QWidget * parent) :
    QLabel(text, parent)
{
}

void ClickableLabel::mousePressEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    emit clicked();
}
