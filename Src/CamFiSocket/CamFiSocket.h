#ifndef CAMFISOCKET_H
#define CAMFISOCKET_H

#include "sio_client.h"
#include <qnetworkaccessmanager.h>
#include <qeventloop.h>
#include <qstring.h>
#include <qjsonarray.h>

//using namespace sio;

class CamFiSocket : public QObject
{
	Q_OBJECT
public:
	CamFiSocket();
	~CamFiSocket();

	sio::client* _io;

	QEventLoop eventLoop;
	QNetworkAccessManager* qnam;
	QString ip;

public:
	void takePicture();
	void startMonitorMode();
	void stopMonitorMode();
	QStringList readPicList(qint16 start, qint16 stop);
	QByteArray downloadRaw(QString filepath);
	QByteArray downloadThumbnail(QString filepath);
	QByteArray downloadPreview(QString filepath);
	bool deleteRaw(QString filepath);
	bool startCaptureMovie();
	bool stopCaptureMovie();
	QJsonDocument getConfig();
	bool setConfig(QString name, QString value);
	void getFirmInfo();

signals:
	void onFileAdded(QString filepath);
	void onFileRemove();
	void onConnected();

private:
	void OnConnected(std::string const& nsp);
	void OnClosed(sio::client::close_reason const& reason);
	void OnFailed();
	void OnFileAdded(std::string const& name, sio::message::ptr const& data, bool hasAck, sio::message::list &ack_resp);
	void OnFileRemove();
};


#endif // CAMFISOCKET_H