#ifndef DISCONNECTTEMPORARILY_H
#define DISCONNECTTEMPORARILY_H

#include <QObject>

class DisconnectTemporarily : public QObject {
    Q_OBJECT
private:
    const QObject   * mSender;
    const char      * mSignal;
    const QObject   * mReceiver;
    const char      * mMember;
public:
    DisconnectTemporarily(const QObject * sender,   const char * signal,
                          const QObject * receiver, const char * member):
    mSender(sender), mSignal(signal), mReceiver(receiver), mMember(member)
    {
        disconnect(mSender, mSignal, mReceiver, mMember);
    }

    ~DisconnectTemporarily() {
        connect(mSender, mSignal, mReceiver, mMember);
    }

};

#endif // DISCONNECTTEMPORARILY_H
