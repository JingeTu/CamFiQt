#ifndef CAMFIQT_GUI_H
#define CAMFIQT_GUI_H
#include <QtWidgets\qmainwindow.h>
#include <qlistwidget.h>
#include "ui_camfiqt_gui.h"

#include "CamFiSocket.h"

#include <qtcpsocket.h>

class CAMFIQT_GUI : public QMainWindow
{
    Q_OBJECT

public:
	CAMFIQT_GUI(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CAMFIQT_GUI();

private:
	Ui::CAMFIQT_GUIClass ui;
	CamFiSocket* cfs;
	QTcpSocket* tcpSocket;
	void updateConfig();
	QString currentIso;
	QString currentShutterSpeed2;
	QString currentFNumber;
	QString currentWhiteBalance;
	QString currentExposureMeterMode;
	QString currentImageQuality;
	QString currentExpProgram;

	QByteArray videoStreamBuffer;

private slots:
	void readMovie();

	void pushButtonTakePic_clicked();
	void pushButtonReadPicList_clicked();
	void pushButtonPicDownloadDirBrowse_clicked();
	void checkBoxSyncFileList_stateChanged(int state);
	void checkBoxCaptureMovie_stateChanged(int state);
	void listWidgetPicList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
	void listWidgetPicList_itemDoubleClicked(QListWidgetItem * item);
	void comboBoxConfigIso_currentTextChanged(const QString & text);
	void checkBoxConfigAutoIso_stateChanged(int state);
	void comboBoxConfigShutterSpeed2_currentTextChanged(const QString & text);
	void comboBoxConfigFNumber_currentTextChanged(const QString & text);
	void comboBoxConfigWhiteBalance_currentTextChanged(const QString & text);
	void comboBoxConfigExposureMeterMode_currentTextChanged(const QString & text);
	void comboBoxConfigImageQuality_currentTextChanged(const QString & text);
	void comboBoxConfigExposureProgram_currentTextChanged(const QString & text);

	void onFileAdded_emited(QString filePath);
	void onConnected_emited();
};

#endif // CAMFIQT_GUI_H
