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


#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "tablemodelpreferences.h"
#include "combodelegate.h"
#include "trackdelegate.h"
#include "timetrackingsettings.h"
#include "debug.h"
#include "misc.h"

#include <QModelIndex>
#include <QMessageBox>
#include <QResizeEvent>
#include <QTableView>
#include <QMultiMap>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
    ui(new Ui::PreferencesDialog),
    mTableWorkingHours(NULL)
{
    ui->setupUi(this);
    mTableWorkingHours = ui->tableViewWorkingHours;
    
    ui->lineEditUserName->setText(Misc::getUserName());
    ui->lineEditRealName->setText(TimeTrackingSettings::getInstance()->getPrintingRealName());
    ui->checkBoxDisplayImage->setChecked(TimeTrackingSettings::getInstance()->getPrintingShowPicture());
    ui->textEditFooter->setText(TimeTrackingSettings::getInstance()->getPrintingFooter());

    ui->checkBoxEnableDefaultActions->setCheckState(
                TimeTrackingSettings::getInstance()->getEnablePeriodDefaultAction() ? Qt::Checked : Qt::Unchecked);
    ui->timeEditDefaultAction->setTime(TimeTrackingSettings::getInstance()->getPeriodDefaultAction());
    connect(ui->timeEditDefaultAction, SIGNAL(timeChanged(QTime)),
            this, SLOT(onTimeChangedDefaultAction(QTime)));
    connect(ui->checkBoxEnableDefaultActions, SIGNAL(stateChanged(int)),
            this, SLOT(onStateChangedDefaultAction(int)));

    ui->checkBoxRemindPauseTime->setCheckState(
                TimeTrackingSettings::getInstance()->getEnablePeriodPauseTime() ? Qt::Checked : Qt::Unchecked);
    ui->timeEditRemindPauseTime->setTime(TimeTrackingSettings::getInstance()->getPeriodPauseTime());
    connect(ui->timeEditRemindPauseTime, SIGNAL(timeChanged(QTime)),
            this, SLOT(onTimeChangedPauseTime(QTime)));
    connect(ui->checkBoxRemindPauseTime, SIGNAL(stateChanged(int)),
            this, SLOT(onStateChangedPauseTime(int)));

    connect(ui->pushButtonAddAction,     SIGNAL(clicked()),
            this, SLOT(onAddAction()));
    connect(ui->pushButtonDeleteAction,  SIGNAL(clicked()),
            this, SLOT(onDeleteAction()));
    connect(ui->tableWidgetActions->selectionModel(),  SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)),
            this, SLOT(onTableActionSelectionChanged(const QModelIndex &, const QModelIndex &)));

    connect(ui->labelWhatIsThis, SIGNAL(clicked()),
            this, SLOT(onWhatIsThis()));

    ui->OKButton->setImagesPath(ClickableImage::mNotSelectedOKButton, ClickableImage::mSelectedOKButtonDefault);
    connect(ui->OKButton, SIGNAL(clicked()),
            this, SLOT(onOK()));

    setAttribute(Qt::WA_TranslucentBackground);

    init();
}

// =============================================================================
PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

// =============================================================================
void PreferencesDialog::init() {
    QAbstractTableModel *model = new TableModelPreferences(this);
    mTableWorkingHours->setModel(model);
    mTableWorkingHours->setItemDelegate(new ComboDelegate(mTableWorkingHours));

    while(ui->tableWidgetActions->rowCount() > 0) {
        ui->tableWidgetActions->removeRow(0);
    }
    const QMultiHash<QString, QString> & actions = TimeTrackingSettings::getInstance()->getActions();
    for(QMultiHash<QString, QString>::ConstIterator
        it = actions.constBegin();
        it != actions.constEnd();
        ++it) {
        int currentRow = ui->tableWidgetActions->rowCount();
        DEBUG(QString("Inserting row %1: %2, %3").arg(currentRow).arg(it.key()).arg(it.value()));
        ui->tableWidgetActions->insertRow(currentRow);
        ui->tableWidgetActions->setItem(currentRow, 0,
                                        new QTableWidgetItem(it.key()));
        ui->tableWidgetActions->setItem(currentRow, 1,
                                        new QTableWidgetItem(it.value()));
        ui->tableWidgetActions->setSelectionBehavior(QAbstractItemView::SelectRows);

    }
}

// =============================================================================
void PreferencesDialog::adjustColumnSizes(int width)
{
    mTableWorkingHours->resizeColumnsToContents();
    width -= mTableWorkingHours->verticalHeader()->size().width() + 20;

    for ( int col = 0; col < mTableWorkingHours->model()->columnCount(); ++col ) {
        width -= mTableWorkingHours->columnWidth( col );
    }
    float widthPerColumn = width / mTableWorkingHours->model()->columnCount();
    for ( int col = 0; col < mTableWorkingHours->model()->columnCount(); ++col ) {
        mTableWorkingHours->setColumnWidth( col,
                                            mTableWorkingHours->columnWidth( col ) +
                                            widthPerColumn );
    }
}

// =============================================================================
void PreferencesDialog::resizeEvent(QResizeEvent* event)
{

    adjustColumnSizes(event->size().width());
    QDialog::resizeEvent(event);

}

// =============================================================================
void PreferencesDialog::onStateChangedDefaultAction(int state) {
    TimeTrackingSettings::getInstance()->setEnablePeriodDefaultAction(state == Qt::Checked);
    ui->lineEditCommand->setEnabled(ui->checkBoxUseDefaultBrowser->checkState() == Qt::Unchecked);
}

// =============================================================================
void PreferencesDialog::onTimeChangedDefaultAction(const QTime &time)
{
    if(time.isValid()) {
        TimeTrackingSettings::getInstance()->setPeriodDefaultAction(time);
    } else {
        DEBUG("Invalid time");
    }
}

// =============================================================================
void PreferencesDialog::onStateChangedPauseTime(int state) {
    TimeTrackingSettings::getInstance()->setEnablePeriodPauseTime(state == Qt::Checked);
}

// =============================================================================
void PreferencesDialog::onAddAction()
{
    if(ui->checkBoxUseDefaultBrowser->checkState() == Qt::Unchecked &&
       ui->lineEditCommand->text().isEmpty()) {
        QMessageBox::information(NULL, QObject::tr("A command must be selected"),
                                 QObject::tr("Please either fill a path for the command to execute or select default browser."));
        return;
    }

    QString parameter = ui->lineEditParameter->text();
    QString command = TimeTrackingSettings::getInstance()->getDefaultBrowserCommand();
    if(ui->checkBoxUseDefaultBrowser->checkState() == Qt::Unchecked) {
        command = ui->lineEditCommand->text();
    }
    int currentRow = ui->tableWidgetActions->rowCount();
    ui->tableWidgetActions->insertRow(currentRow);
    ui->tableWidgetActions->setItem(currentRow, 0,
                                    new QTableWidgetItem(command));
    ui->tableWidgetActions->setItem(currentRow, 1,
                                    new QTableWidgetItem(parameter));
    TimeTrackingSettings::getInstance()->addAction(command, parameter);
    TimeTrackingSettings::getInstance()->informClients();

}

// =============================================================================
void PreferencesDialog::onDeleteAction()
{
    QTableWidgetItem * itemCommand = ui->tableWidgetActions->item(
                ui->tableWidgetActions->currentRow(), 0);
    QTableWidgetItem * itemParameter = ui->tableWidgetActions->item(
                ui->tableWidgetActions->currentRow(), 1);
    TimeTrackingSettings::getInstance()->removeAction(itemCommand->text(),
                                                      itemParameter->text());
    ui->tableWidgetActions->removeRow(ui->tableWidgetActions->currentRow());
    TimeTrackingSettings::getInstance()->informClients();

}

// =============================================================================
void PreferencesDialog::onTableActionSelectionChanged(const QModelIndex & current,
                                                      const QModelIndex & previous)
{
    Q_UNUSED(previous);
    ui->pushButtonDeleteAction->setEnabled(current.isValid());
}

// =============================================================================
void PreferencesDialog::onWhatIsThis()
{
    QMessageBox::information(NULL, QObject::tr("Configuration of periodic tasks"),
                             QObject::tr("This tool allows you to configure any tasks to be started "
                                         "on regular bases activated only during working time.\n\n"
                                         "The configuration is very easy, in the field command, "
                                         "provide the path to an executable (a command), alternatively "
                                         "click on Default Browser if the command should actually start "
                                         "the default browser.\n\n"
                                         "The parameter is either a web page (if the command is a browser) "
                                         "or a film you want to play (if the command is a movie player).\n\n"
                                         "Please find more information on the help (accessible on http://twoblackcoffees.com)."));

}

// =============================================================================
void PreferencesDialog::onOK()
{
    TimeTrackingSettings::getInstance()->setPrintingRealName(ui->lineEditRealName->text());
    TimeTrackingSettings::getInstance()->setPrintingShowPicture(ui->checkBoxDisplayImage->checkState() == Qt::Checked);
    TimeTrackingSettings::getInstance()->setPrintingFooter(ui->textEditFooter->toPlainText());

    accept();
}

// =============================================================================
void PreferencesDialog::onTimeChangedPauseTime(const QTime &time)
{
    if(time.isValid()) {
        TimeTrackingSettings::getInstance()->setPeriodPauseTime(time);
    } else {
        DEBUG("Invalid time for pause");
    }
}
