#include "camfiqt_gui.h"
#include <iostream>
#include <fstream>
#include <qgraphicsitem.h>
#include <qfile.h>
#include <qfiledialog.h>

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qcolor.h>


static const char jpegHeaderChar[] = { 0xff, 0xd8 };

static const QByteArray jpegHeader = QByteArray::fromRawData(jpegHeaderChar, sizeof(jpegHeaderChar));

static const char jpegTailChar[] = { 0x00, 0x00, 0xff, 0xd8 };

static const QByteArray jpegTail = QByteArray::fromRawData(jpegTailChar, sizeof(jpegTailChar));

CAMFIQT_GUI::CAMFIQT_GUI(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);

	cfs = new CamFiSocket();
	tcpSocket = new QTcpSocket(this);

	if (ui.checkBoxSyncFileList->isChecked() == true) ui.pushButtonReadPicList->setEnabled(false);

	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMovie()));

	connect(ui.pushButtonTakePic, SIGNAL(clicked()), this, SLOT(pushButtonTakePic_clicked()));
	connect(ui.pushButtonReadPicList, SIGNAL(clicked()), this, SLOT(pushButtonReadPicList_clicked()));
	connect(ui.pushButtonPicDownloadDirBrowse, SIGNAL(clicked()), this, SLOT(pushButtonPicDownloadDirBrowse_clicked()));

	connect(ui.checkBoxSyncFileList, SIGNAL(stateChanged(int)), this, SLOT(checkBoxSyncFileList_stateChanged(int)));
	connect(ui.checkBoxCaptureMovie, SIGNAL(stateChanged(int)), this, SLOT(checkBoxCaptureMovie_stateChanged(int)));
	connect(ui.listWidgetPicList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(listWidgetPicList_currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
	connect(ui.listWidgetPicList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listWidgetPicList_itemDoubleClicked(QListWidgetItem*)));
	connect(ui.comboBoxConfigIso, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigIso_currentTextChanged(const QString &)));
	connect(ui.checkBoxConfigAutoIso, SIGNAL(stateChanged(int)), this, SLOT(checkBoxConfigAutoIso_stateChanged(int)));
	connect(ui.comboBoxConfigShutterSpeed2, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigShutterSpeed2_currentTextChanged(const QString &)));
	connect(ui.comboBoxConfigFNumber, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigFNumber_currentTextChanged(const QString &)));
	connect(ui.comboBoxConfigWhiteBalance, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigWhiteBalance_currentTextChanged(const QString &)));
	connect(ui.comboBoxConfigExposureMeterMode, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigExposureMeterMode_currentTextChanged(const QString &)));
	connect(ui.comboBoxConfigImageQuality, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigImageQuality_currentTextChanged(const QString &)));
	connect(ui.comboBoxConfigExposureProgram, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxConfigExposureProgram_currentTextChanged(const QString &)));

	connect(cfs, SIGNAL(onFileAdded(QString)), this, SLOT(onFileAdded_emited(QString)));
	connect(cfs, SIGNAL(onConnected()), this, SLOT(onConnected_emited()));
}

CAMFIQT_GUI::~CAMFIQT_GUI()
{

}

void CAMFIQT_GUI::readMovie()
{
	QByteArray qba = tcpSocket->readAll();

	videoStreamBuffer.append(qba);

	int from = videoStreamBuffer.indexOf(jpegHeader);

	int to = videoStreamBuffer.indexOf(jpegTail, from);

	if (from != -1 && to != -1)
	{
		QByteArray buffer = videoStreamBuffer.left(to + 2);
		QByteArray jpegbuffer = buffer.right(to + 2 - from);
		QPixmap qpm;
		qpm.loadFromData(jpegbuffer);
		QGraphicsPixmapItem item(qpm);
		ui.labelLiveMoive->setPixmap(qpm);

		videoStreamBuffer.remove(0, to + 2);
	}
}

void CAMFIQT_GUI::pushButtonTakePic_clicked()
{
	cfs->takePicture();
}

void CAMFIQT_GUI::updateConfig()
{
	QJsonDocument doc = cfs->getConfig();

	QJsonObject obj = doc.object();

	// batterylevel
	
	QJsonObject batterylevelObj = obj.value("main").toObject().value("children").toObject().value("status").toObject().value("children").toObject().value("batterylevel").toObject();

	ui.labelConfigCameraBatteryLevel->setText(batterylevelObj.value("value").toString());

	// iso

	QJsonObject isoObj = obj.value("main").toObject().value("children").toObject().value("imgsettings").toObject().value("children").toObject().value("iso").toObject();

	QJsonArray isoChoices = isoObj.value("choices").toArray();

	ui.comboBoxConfigIso->clear();

	for (int i = 0; i < isoChoices.size(); i++)
	{
		ui.comboBoxConfigIso->addItem(isoChoices.at(i).toString());
	}

	ui.comboBoxConfigIso->setCurrentIndex(ui.comboBoxConfigIso->findText(isoObj.value("value").toString()));
	currentIso = ui.comboBoxConfigIso->currentText();

	// autoiso
	
	QJsonObject autoisoObj = obj.value("main").toObject().value("children").toObject().value("imgsettings").toObject().value("children").toObject().value("autoiso").toObject();

	//QJsonArray autoisoChoices = autoisoObj.value("choices").toArray();

	if (autoisoObj.value("value").toString() == "On")
	{
		ui.checkBoxConfigAutoIso->setCheckState(Qt::CheckState::Checked);
		ui.comboBoxConfigIso->setEnabled(false);
	}
	else if (autoisoObj.value("value").toString() == "Off")
	{
		ui.checkBoxConfigAutoIso->setCheckState(Qt::CheckState::Unchecked);
		ui.comboBoxConfigIso->setEnabled(true);
	}
	
	// shutterpseed2

	QJsonObject shutterspeed2Obj = obj.value("main").toObject().value("children").toObject().value("capturesettings").toObject().value("children").toObject().value("shutterspeed2").toObject();

	QJsonArray shutterspeed2Choices = shutterspeed2Obj.value("choices").toArray();

	ui.comboBoxConfigShutterSpeed2->clear();

	for (int i = 0; i < shutterspeed2Choices.size(); i++)
	{
		ui.comboBoxConfigShutterSpeed2->addItem(shutterspeed2Choices.at(i).toString());
	}

	ui.comboBoxConfigShutterSpeed2->setCurrentIndex(ui.comboBoxConfigShutterSpeed2->findText(shutterspeed2Obj.value("value").toString()));
	currentShutterSpeed2 = ui.comboBoxConfigShutterSpeed2->currentText();
	
	// f-number

	QJsonObject fnumberObj = obj.value("main").toObject().value("children").toObject().value("capturesettings").toObject().value("children").toObject().value("f-number").toObject();

	QJsonArray fnumberChoices = fnumberObj.value("choices").toArray();

	ui.comboBoxConfigFNumber->clear();

	for (int i = 0; i < fnumberChoices.size(); i++)
	{
		ui.comboBoxConfigFNumber->addItem(fnumberChoices.at(i).toString());
	}

	ui.comboBoxConfigFNumber->setCurrentIndex(ui.comboBoxConfigFNumber->findText(fnumberObj.value("value").toString()));
	currentFNumber = ui.comboBoxConfigFNumber->currentText();
	
	// whitebalance

	QJsonObject whitebalanceObj = obj.value("main").toObject().value("children").toObject().value("imgsettings").toObject().value("children").toObject().value("whitebalance").toObject();

	QJsonArray whitebalanceChoices = whitebalanceObj.value("choices").toArray();

	ui.comboBoxConfigWhiteBalance->clear();

	for (int i = 0; i < whitebalanceChoices.size(); i++)
	{
		ui.comboBoxConfigWhiteBalance->addItem(whitebalanceChoices.at(i).toString());
	}

	ui.comboBoxConfigWhiteBalance->setCurrentIndex(ui.comboBoxConfigWhiteBalance->findText(whitebalanceObj.value("value").toString()));
	currentWhiteBalance = ui.comboBoxConfigWhiteBalance->currentText();
	
	// exposuremetermode

	QJsonObject exposuremetermodeObj = obj.value("main").toObject().value("children").toObject().value("capturesettings").toObject().value("children").toObject().value("exposuremetermode").toObject();

	QJsonArray exposuremetermodeChoices = exposuremetermodeObj.value("choices").toArray();

	ui.comboBoxConfigExposureMeterMode->clear();

	for (int i = 0; i < exposuremetermodeChoices.size(); i++)
	{
		ui.comboBoxConfigExposureMeterMode->addItem(exposuremetermodeChoices.at(i).toString());
	}

	ui.comboBoxConfigExposureMeterMode->setCurrentIndex(ui.comboBoxConfigExposureMeterMode->findText(exposuremetermodeObj.value("value").toString()));
	currentExposureMeterMode = ui.comboBoxConfigExposureMeterMode->currentText();
	
	// imagequality

	QJsonObject imagequalityObj = obj.value("main").toObject().value("children").toObject().value("capturesettings").toObject().value("children").toObject().value("imagequality").toObject();

	QJsonArray imagequalityChoices = imagequalityObj.value("choices").toArray();

	ui.comboBoxConfigImageQuality->clear();

	for (int i = 0; i < imagequalityChoices.size(); i++)
	{
		ui.comboBoxConfigImageQuality->addItem(imagequalityChoices.at(i).toString());
	}

	ui.comboBoxConfigImageQuality->setCurrentIndex(ui.comboBoxConfigImageQuality->findText(imagequalityObj.value("value").toString()));
	currentImageQuality = ui.comboBoxConfigImageQuality->currentText();

	// expprogram

	QJsonObject expprogramObj = obj.value("main").toObject().value("children").toObject().value("capturesettings").toObject().value("children").toObject().value("expprogram").toObject();

	QJsonArray expprogramChoices = expprogramObj.value("choices").toArray();
	
	ui.comboBoxConfigExposureProgram->clear();

	for (int i = 0; i < expprogramChoices.size(); i++)
	{
		ui.comboBoxConfigExposureProgram->addItem(expprogramChoices.at(i).toString());
	}

	ui.comboBoxConfigExposureProgram->setCurrentIndex(ui.comboBoxConfigExposureProgram->findText(expprogramObj.value("value").toString()));
	currentExpProgram = ui.comboBoxConfigExposureProgram->currentText();


}

void CAMFIQT_GUI::pushButtonReadPicList_clicked()
{
	QStringList qsl = cfs->readPicList(ui.lineEditReadPicFrom->text().toInt(), ui.lineEditReadPicTo->text().toInt());
	ui.listWidgetPicList->clear();
	ui.listWidgetPicList->addItems(qsl);
}

void CAMFIQT_GUI::pushButtonPicDownloadDirBrowse_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"G:\PROJECT\DevicesMaterials\CamFi\CamFiQt\Data",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	ui.lineEditDownloadDirectory->setText(dir);
}

void CAMFIQT_GUI::checkBoxSyncFileList_stateChanged(int state)
{
	if (state == Qt::CheckState::Checked)
	{
		ui.pushButtonReadPicList->setEnabled(false);
	}
	else if (state == Qt::CheckState::Unchecked)
	{
		ui.pushButtonReadPicList->setEnabled(true);
	}
}

void CAMFIQT_GUI::checkBoxCaptureMovie_stateChanged(int state)
{
	if (state == Qt::CheckState::Checked)
	{
		bool succeed = cfs->startCaptureMovie();
		if (succeed)
		{
			tcpSocket->connectToHost(cfs->ip, 890, QIODevice::ReadOnly);
		}
		else
		{
			ui.checkBoxCaptureMovie->setCheckState(Qt::CheckState::Unchecked);
		}
	}
	else if (state == Qt::CheckState::Unchecked)
	{
		bool succeed = cfs->startCaptureMovie();
		if (succeed)
		{
			tcpSocket->disconnectFromHost();
		}
		else
		{
			ui.checkBoxCaptureMovie->setCheckState(Qt::CheckState::Checked);
		}
	}
}

void CAMFIQT_GUI::listWidgetPicList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
	if (current != NULL)
	{
		QString qs = current->text();
		qs = qs.replace("/", "%2F");

		QByteArray qba = cfs->downloadThumbnail(qs);

		QPixmap qpm;
		qpm.loadFromData(qba);
		QGraphicsPixmapItem item(qpm);
		ui.labelThumbnail->setPixmap(qpm);
	}
}

void CAMFIQT_GUI::listWidgetPicList_itemDoubleClicked(QListWidgetItem * item)
{
	if (ui.radioButtonDoubleClickToDownload->isChecked() == true && item->backgroundColor() != Qt::gray)
	{
		QString qs = item->text();
		qs = qs.replace("/", "%2F");

		QByteArray qba = cfs->downloadRaw(qs);

		QFile* file = new QFile(ui.lineEditDownloadDirectory->text() + qs.split("%2F").last());
		file->open(QIODevice::WriteOnly);
		file->write(qba);
		file->close();
	}
	else if (ui.radioButtonDoubleClickToDelete->isChecked() == true)
	{
		QString qs = item->text();
		qs = qs.replace("/", "%2F");

		bool succeed = cfs->deleteRaw(qs);

		if (succeed)
		{
			item->setBackgroundColor(Qt::gray);
		}
	}
}

void CAMFIQT_GUI::comboBoxConfigIso_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("iso", text);
	if (succeed)
	{
		currentIso = ui.comboBoxConfigIso->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigIso->setCurrentIndex(ui.comboBoxConfigIso->findText(currentIso));
	}
}

void CAMFIQT_GUI::checkBoxConfigAutoIso_stateChanged(int state)
{
	if (state == Qt::CheckState::Checked)
	{
		if (cfs->setConfig("autoiso", "On"))
		{
			ui.comboBoxConfigIso->setEnabled(false);
			ui.checkBoxConfigAutoIso->setCheckState(Qt::CheckState::Checked);
		}
		else
		{
			ui.comboBoxConfigIso->setEnabled(true);
			ui.checkBoxConfigAutoIso->setCheckState(Qt::CheckState::Unchecked);
		}
	}
	else if (state == Qt::CheckState::Unchecked)
	{
		if (cfs->setConfig("autoiso", "Off"))
		{
			ui.comboBoxConfigIso->setEnabled(true);
			ui.checkBoxConfigAutoIso->setCheckState(Qt::CheckState::Unchecked);
		}
		else
		{
			ui.comboBoxConfigIso->setEnabled(false);
			ui.checkBoxConfigAutoIso->setCheckState(Qt::CheckState::Checked);
		}
	}
}

void CAMFIQT_GUI::comboBoxConfigShutterSpeed2_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("shutterspeed2", text);
	if (succeed)
	{
		currentShutterSpeed2 = ui.comboBoxConfigShutterSpeed2->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigShutterSpeed2->setCurrentIndex(ui.comboBoxConfigShutterSpeed2->findText(currentShutterSpeed2));
	}
}

void CAMFIQT_GUI::comboBoxConfigFNumber_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("f-number", text);
	if (succeed)
	{
		currentFNumber = ui.comboBoxConfigFNumber->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigFNumber->setCurrentIndex(ui.comboBoxConfigFNumber->findText(currentFNumber));
	}
}

void CAMFIQT_GUI::comboBoxConfigWhiteBalance_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("whitebalance", text);
	if (succeed)
	{
		currentWhiteBalance = ui.comboBoxConfigWhiteBalance->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigWhiteBalance->setCurrentIndex(ui.comboBoxConfigWhiteBalance->findText(currentWhiteBalance));
	}
}

void CAMFIQT_GUI::comboBoxConfigExposureMeterMode_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("exposuremetermode", text);
	if (succeed)
	{
		currentExposureMeterMode = ui.comboBoxConfigExposureMeterMode->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigExposureMeterMode->setCurrentIndex(ui.comboBoxConfigExposureMeterMode->findText(currentExposureMeterMode));
	}
}

void CAMFIQT_GUI::comboBoxConfigImageQuality_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("imagequality", text);
	if (succeed)
	{
		currentImageQuality = ui.comboBoxConfigImageQuality->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigImageQuality->setCurrentIndex(ui.comboBoxConfigImageQuality->findText(currentImageQuality));
	}
}

void CAMFIQT_GUI::comboBoxConfigExposureProgram_currentTextChanged(const QString & text)
{
	bool succeed = cfs->setConfig("expprogram", text);
	if (succeed)
	{
		currentExpProgram = ui.comboBoxConfigExposureProgram->currentText();
	}
	else
	{
		// roll back
		ui.comboBoxConfigExposureProgram->setCurrentIndex(ui.comboBoxConfigExposureProgram->findText(currentExpProgram));
	}
}

void CAMFIQT_GUI::onFileAdded_emited(QString filePath)
{
	if (ui.checkBoxSyncFileList->isChecked() == true)
	{
		ui.listWidgetPicList->addItem(filePath);
	}
}
void CAMFIQT_GUI::onConnected_emited()
{
	updateConfig();
}