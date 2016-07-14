#include "CamFiSocket.h"

#include <iostream>
#include <qnetworkrequest.h>
#include <qnetworkreply.h>
#include <qurl.h>
#include <qurlquery.h>
#include <qbytearray.h>
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

#include <qvariant.h>
#include <qhttpmultipart.h>

#define CFIP "192.168.9.67"
#ifdef WIN32
#define BIND_EVENT(IO,EV,FN) \
    do{ \
        sio::socket::event_listener_aux l = FN;\
        IO->on(EV,l);\
    } while(0)

#else
#define BIND_EVENT(IO,EV,FN) \
    IO->on(EV,FN)
#endif

CamFiSocket::CamFiSocket()
{
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;
	using std::placeholders::_4;
	_io = new sio::client();
	sio::socket::ptr sock = _io->socket();
	//BIND_EVENT(sock, "new message", std::bind(&CamFiSocket::OnNewMessage, this, _1, _2, _3, _4));
	//BIND_EVENT(sock, "user joined", std::bind(&CamFiSocket::OnUserJoined, this, _1, _2, _3, _4));
	//BIND_EVENT(sock, "user left", std::bind(&CamFiSocket::OnUserLeft, this, _1, _2, _3, _4));
	//BIND_EVENT(sock, "typing", std::bind(&CamFiSocket::OnTyping, this, _1, _2, _3, _4));
	//BIND_EVENT(sock, "stop typing", std::bind(&CamFiSocket::OnStopTyping, this, _1, _2, _3, _4));
	//BIND_EVENT(sock, "file_added", std::bind(&CamFiSocket::OnFileAdded, this, _1, _2));
	BIND_EVENT(sock, "file_added", std::bind(&CamFiSocket::OnFileAdded, this, _1, _2, _3, _4));
	BIND_EVENT(sock, "file_remove", std::bind(&CamFiSocket::OnFileRemove, this));

	_io->set_socket_open_listener(std::bind(&CamFiSocket::OnConnected, this, std::placeholders::_1));
	_io->set_close_listener(std::bind(&CamFiSocket::OnClosed, this, _1));
	_io->set_fail_listener(std::bind(&CamFiSocket::OnFailed, this));

	ip = CFIP;
	QString wsUrl = "http://" + ip + ":8080";
	std::cout << wsUrl.toStdString() << "\n";
	//_io->connect("http://192.168.9.67:8080");
	_io->connect(wsUrl.toStdString());

	qnam = new QNetworkAccessManager();
	QObject::connect(qnam, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
}

CamFiSocket::~CamFiSocket()
{
	_io->socket()->off_all();
	_io->socket()->off_error();
}

void CamFiSocket::takePicture()
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/takepic/true")));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();
}

void CamFiSocket::startMonitorMode()
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/tether/start")));
	QNetworkReply *reply = qnam->post(req, "");
	eventLoop.exec();
}

void CamFiSocket::stopMonitorMode()
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/tether/stop")));
	QNetworkReply *reply = qnam->post(req, "");
	eventLoop.exec();
}

QStringList CamFiSocket::readPicList(qint16 start, qint16 stop)
{
	QStringList qsl;

	QNetworkRequest req(QUrl(QString("http://" + ip + "/files/" + QString::number(start) + "/" + QString::number(stop))));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		QString dataString = reply->readAll();
		//qDebug() << "Success" << dataString;
		QJsonDocument d = QJsonDocument::fromJson(dataString.toUtf8());
		QJsonArray filesJsonArray = d.array();

		for (size_t i = 0; i < filesJsonArray.size(); i++)
		{
			QJsonValue value = filesJsonArray.at(i);
			qsl.append(value.toString());
		}

		delete reply;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}

	return qsl;
}

QByteArray CamFiSocket::downloadRaw(QString filepath)
{
	QByteArray qba;
	QNetworkRequest req(QUrl(QString("http://" + ip + "/raw/" + filepath)));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qba = reply->readAll();
		delete reply;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}

	return qba;
}

QByteArray CamFiSocket::downloadThumbnail(QString filepath)
{
	QByteArray qba;
	QNetworkRequest req(QUrl(QString("http://" + ip + "/thumbnail/" + filepath)));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qba = reply->readAll();
		delete reply;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}

	return qba;
}

QByteArray CamFiSocket::downloadPreview(QString filepath)
{
	QByteArray qba;
	QNetworkRequest req(QUrl(QString("http://" + ip + "/image/" + filepath)));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qba = reply->readAll();
		delete reply;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}

	return qba;
}

bool CamFiSocket::deleteRaw(QString filepath)
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/image/" + filepath)));
	QNetworkReply *reply = qnam->deleteResource(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qDebug() << "Success" << reply->readAll();
		delete reply;
		return true;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
		return false;
	}
}

bool CamFiSocket::startCaptureMovie()
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/capturemovie")));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qDebug() << "Success" << reply->readAll();
		delete reply;
		return true;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
		return false;
	}
}

bool CamFiSocket::stopCaptureMovie()
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/stopcapturemovie")));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qDebug() << "Success" << reply->readAll();
		delete reply;
		return true;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
		return false;
	}
}

QJsonDocument CamFiSocket::getConfig()
{
	QJsonDocument doc;
	QNetworkRequest req(QUrl(QString("http://" + ip + "/config")));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		//qDebug() << "Success" << reply->readAll();
		QString dataString = reply->readAll();

		doc = QJsonDocument::fromJson(dataString.toUtf8());
		delete reply;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}
	return doc;
}

bool CamFiSocket::setConfig(QString name, QString value)
{
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart textPart;
	textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
	textPart.setBody(name.toUtf8());
	

	QHttpPart textPart1;
	textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"value\""));
	textPart1.setBody(value.toUtf8());

	multiPart->append(textPart);
	multiPart->append(textPart1);

	QNetworkRequest req(QUrl(QString("http://" + ip + "/setconfigvalue")));
	QNetworkReply* reply = qnam->put(req, multiPart);

	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qDebug() << "Success" << reply->readAll();
		delete reply;
		return true;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
		return false;
	}
}

void CamFiSocket::getFirmInfo()
{
	QNetworkRequest req(QUrl(QString("http://" + ip + "/info")));
	QNetworkReply *reply = qnam->get(req);
	eventLoop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qDebug() << "Success" << reply->readAll();
		delete reply;
	}
	else {
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}
}

void CamFiSocket::OnConnected(std::string const & nsp)
{
	std::cout << "OnConnected\n";
	emit onConnected();
}

void CamFiSocket::OnClosed(sio::client::close_reason const & reason)
{
	std::cout << "OnClosed\n";
}

void CamFiSocket::OnFailed()
{
	std::cout << "OnFailed\n";
}

void CamFiSocket::OnFileAdded(std::string const& name, sio::message::ptr const& data, bool hasAck, sio::message::list &ack_resp)
{
	// data->get_map().size()
	// data->get_map()[0]
	
	std::cout << "take picture successed, name: " << name << ", data: " << data->get_string() << ".\n";
	// /foo.jpg
	if(data->get_string() != "/foo.jpg")
		emit onFileAdded(QString::fromStdString(data->get_string()));
}

void CamFiSocket::OnFileRemove()
{
	std::cout << "one picture removed.\n";
}
