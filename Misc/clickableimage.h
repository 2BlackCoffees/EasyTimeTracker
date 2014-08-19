#ifndef CLICKABLEIMAGE_H
#define CLICKABLEIMAGE_H

#include <QLabel>
#include <memory>

class ClickableImage : public QLabel
{
    Q_OBJECT

public:
    static const QString mSelectedOKButtonDefault;
    static const QString mSelectedOKButtonDollar;
    static const QString mNotSelectedOKButton;
    explicit ClickableImage(QWidget *parent = nullptr);
    void setImagesPath(const QString & imagePathNotSelected, const QString & imagePathSelected);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent * event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QString mImagePathNotSelected;
    QString mImagePathSelected;

signals:
    void clicked();

public slots:

};

#endif // CLICKABLEIMAGE_H
