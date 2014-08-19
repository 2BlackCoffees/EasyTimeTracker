#include "clickableimage.h"

const QString ClickableImage::mSelectedOKButtonDefault = ":/images/tt_ok_button_selected_default.png";
const QString ClickableImage::mSelectedOKButtonDollar = ":/images/tt_ok_button_selected_dollar.png";
const QString ClickableImage::mNotSelectedOKButton = ":/images/tt_ok_button_not_selected.png";

ClickableImage::ClickableImage(QWidget *parent) : QLabel(parent)
{
}

void ClickableImage::setImagesPath(const QString & imagePathNotSelected, const QString & imagePathSelected)
{
    mImagePathNotSelected = imagePathNotSelected;
    mImagePathSelected = imagePathSelected;
    setPixmap(QPixmap(mImagePathNotSelected));

}

void ClickableImage::enterEvent(QEvent *event) {
    setPixmap(QPixmap(mImagePathSelected));
}

void ClickableImage::leaveEvent(QEvent * event) {
    setPixmap(QPixmap(mImagePathNotSelected));

}

void ClickableImage::mouseReleaseEvent(QMouseEvent* event) {
  emit clicked();
}

