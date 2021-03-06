#ifndef PAYMENTSERVER_H
#define PAYMENTSERVER_H

//
// This class handles payment requests from clicking on
// bitcoin: URIs
//
// This is somewhat tricky, because we have to deal with
// the situation where the user clicks on a link during
// startup/initialization, when the splash-screen is up
// but the main window (and the Send Coins tab) is not.
//
// So, the strategy is:
//
// Create the server, and register the event handler,
// when the application is created. Save any URIs
// received at or during startup in a list.
//
// When startup is finished and the main window is
// show, a signal is sent to slot uiReady(), which
// emits a receivedURL() signal for any payment
// requests that happened during startup.
//
// After startup, receivedURL() happens as usual.
//
// This class has one more feature: a static
// method that finds URIs passed in the command line
// and, if a server is running in another process,
// sends them to the server.
//
#include <QObject>
#include <QString>

#include "paymentrequestplus.h"
#include "walletmodel.h"

class CWallet;
class OptionsModel;
class QApplication;
class QByteArray;
class QLocalServer;
class QNetworkAccessManager;
class QNetworkReply;
class QSslError;
class QUrl;

class PaymentServer : public QObject
{
    Q_OBJECT

public:
    // Returns true if there were URIs on the command line
    // which were successfully sent to an already-running
    // process.
    // Note: if a payment request is given, SelectParams(MAIN/TESTNET)
    // will be called so we startup in the right mode.
    static bool ipcSendCommandLine(int argc, char *argv[]);

    PaymentServer(QObject* parent, // parent should be QApplication object
                  bool startLocalServer=true);
    ~PaymentServer();

    // Load root certificate authorities. Pass NULL (default)
    // to read from the file specified in the -rootcertificates setting,
    // or, if that's not set, to use the system default root certificates.
    // If you pass in a store, you should not X509_STORE_free it: it will be
    // freed either at exit or when another set of CAs are loaded.
    static void LoadRootCAs(X509_STORE* store=NULL);

    // Return certificate store
    static X509_STORE* getCertStore() { return certStore; }

    // Setup networking (options is used to get proxy settings)
    void initNetManager(const OptionsModel& options);

    // Constructor registers this on the parent QApplication to
    // receive QEvent::FileOpen events
    bool eventFilter(QObject *object, QEvent *event);

signals:
    // Fired when a valid payment request is received
    void receivedPaymentRequest(SendCoinsRecipient);

    // Fired when a valid PaymentACK is received
    void receivedPaymentACK(QString);

    // Fired when an error should be reported to the user
    void reportError(QString, QString, unsigned int);

public slots:
    // Signal this when the main window's UI is ready
    // to display payment requests to the user
    void uiReady();

    // Submit Payment message to a merchant, get back PaymentACK:
    void fetchPaymentACK(CWallet* wallet, SendCoinsRecipient recipient, QByteArray transaction);

private slots:
    void handleURIConnection();
    void netRequestFinished(QNetworkReply*);
    void reportSslErrors(QNetworkReply*, const QList<QSslError> &);

private:
    static bool readPaymentRequest(const QString& filename, PaymentRequestPlus& request);
    bool processPaymentRequest(PaymentRequestPlus& request, QList<SendCoinsRecipient>& recipients);
    void handleURIOrFile(const QString& s);
    void fetchRequest(const QUrl& url);

    bool saveURIs;                 // true during startup
    QLocalServer* uriServer;
    static X509_STORE* certStore; // Trusted root certificates
    static void freeCertStore();

    QNetworkAccessManager* netManager; // Used to fetch payment requests
};

#endif // PAYMENTSERVER_H
