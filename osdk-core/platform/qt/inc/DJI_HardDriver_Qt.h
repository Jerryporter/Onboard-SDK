/*! @file DJI_HardDriver_Qt.h
 *  @version 3.1.7
 *  @date Jul 01 2016
 *
 *  @brief
 *  Serial device hardware abstraction for DJI Onboard SDK Qt example.
 *
 *  @note New Qt sample coming soon!
 *
 *  @copyright
 *  2016 DJI. All rights reserved.
 * */


#ifndef QONBOARDSDK_H
#define QONBOARDSDK_H

#include <DJI_API.h>
#include <DJI_Flight.h>
#include <DJI_Follow.h>
#include <DJI_HardDriver.h>
#include <DJI_VirtualRC.h>
#include <QComboBox>
#include <QItemDelegate>
#include <QMutex>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <dji_camera.hpp>
#include <dji_hot_point.hpp>
#include <dji_waypoint.hpp>

#include <QTextBrowser>

//! @warning not good to use using in header file
using namespace DJI::onboardSDK;

class QHardDriver : public HardDriver
{
public:
  QHardDriver(QSerialPort* Port);

  void         init();
  DJI::OSDK::time_ms getTimeStamp();
  size_t send(const uint8_t* buf, size_t len);
  size_t readall(uint8_t* buf, size_t maxlen);

  void lockMemory();
  void freeMemory();

  void lockMSG();
  void freeMSG();

  void displayLog(const char* buf = 0);

  void setBaudrate(int value);

  QTextBrowser* getDisplay() const;
  void setDisplay(QTextBrowser* value);

private:
  QHardDriver();

private:
  int           baudrate;
  QSerialPort*  port;
  QMutex        memory;
  QMutex        msg;
  QMutex        sendLock;
  QMutex        bufferLock;
  QTextBrowser* display;
};

class APIThread : public QThread
{
  Q_OBJECT

public:
  APIThread();
  APIThread(CoreAPI* API, int Type, QObject* parent = 0);

  void run();

  size_t getCallTimes() const;
  void setCallTimes(const size_t& value);

private:
  CoreAPI* api;
  int      type;
  size_t   callTimes;
};

//! @note widget for GUI
class TurnModeDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  TurnModeDelegate(QObject* parent = 0)
    : QItemDelegate(parent)
  {
  }
  QWidget* createEditor(QWidget*                    parent,
                        const QStyleOptionViewItem& option __UNUSED,
                        const QModelIndex& index __UNUSED) const
  {
    QComboBox* editor = new QComboBox(parent);
    editor->addItem("Clockwise");
    editor->addItem("Counter-clockwise");
    return editor;
  }
  void setEditorData(QWidget* editor, const QModelIndex& index) const
  {
    QString    text     = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    int        tindex   = comboBox->findText(text);
    comboBox->setCurrentIndex(tindex);
  }
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const
  {
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    QString    text     = comboBox->currentText();
    model->setData(index, text, Qt::EditRole);
  }
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index __UNUSED) const
  {
    editor->setGeometry(option.rect);
  }
};

class ActionDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  ActionDelegate(QObject* parent = 0)
    : QItemDelegate(parent)
  {
  }
  QWidget* createEditor(QWidget*                    parent,
                        const QStyleOptionViewItem& option __UNUSED,
                        const QModelIndex& index __UNUSED) const
  {
    QComboBox* editor = new QComboBox(parent);
    editor->addItem("Stay");
    editor->addItem("Take picture");
    editor->addItem("Start recording");
    editor->addItem("Stop recording");
    editor->addItem("Yaw");
    editor->addItem("Gimbal pitch");
    return editor;
  }
  void setEditorData(QWidget* editor, const QModelIndex& index) const
  {
    QString    text     = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    int        tindex   = comboBox->findText(text);
    comboBox->setCurrentIndex(tindex);
  }
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const
  {
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    QString    text     = comboBox->currentText();
    model->setData(index, text, Qt::EditRole);
  }
  void updateEditorGeometry(QWidget*                    editor,
                            const QStyleOptionViewItem& option __UNUSED,
                            const QModelIndex& index __UNUSED) const
  {
    editor->setGeometry(option.rect);
  }
};

class ReadOnlyDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  ReadOnlyDelegate(QObject* parent = 0)
    : QItemDelegate(parent)
  {
  }
  QWidget* createEditor(QWidget* parent             __UNUSED,
                        const QStyleOptionViewItem& option __UNUSED,
                        const QModelIndex& index __UNUSED) const
  {
    return NULL;
  }
};

#endif // QONBOARDSDK_H
