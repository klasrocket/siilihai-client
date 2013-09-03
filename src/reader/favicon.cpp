#include "favicon.h"
#include <siilihai/forumdata/forumthread.h>
#include <siilihai/forumdata/forumsubscription.h>

Favicon::Favicon(QObject *parent, ForumSubscription *sub) : QObject(parent) {
    subscription = sub;
    currentProgress = 0;
//    engineSet = false;
    reloading = false;
    connect(sub, SIGNAL(changed()), this, SLOT(subscriptionChanged()));
    connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(update()));
    blinkTimer.setInterval(100);
    blinkTimer.setSingleShot(false);
    subscriptionChanged();
}

Favicon::~Favicon() {
}

void Favicon::subscriptionChanged() {
    if(!subscription) return;
    //   if(engineSet) return;
    /*
    if(subscription->updateEngine()) {
        engineSet = true;
        connect(subscription->updateEngine(),
                SIGNAL(stateChanged(UpdateEngine::UpdateEngineState, UpdateEngine::UpdateEngineState)),
                this,
                SLOT(engineStateChanged(UpdateEngine::UpdateEngineState)));
        engineStateChanged(subscription->updateEngine(), subscription->updateEngine()->state());
    }
    */
    bool shouldBlink = subscription->scheduledForUpdate() || subscription->beingSynced() || subscription->beingUpdated();
    if(shouldBlink) {
        update();
        blinkTimer.start();
    } else {
        blinkAngle = 0;
        blinkTimer.stop();
        emit iconChanged(subscription, currentpic);
    }
    if(subscription->updateEngine() && subscription->updateEngine()->state()==ParserEngine::UES_ERROR) {
        QPixmap outPic(currentpic);
        QPainter painter(&outPic);
        painter.drawPixmap(0,0,outPic.width(),outPic.height(), QPixmap(":data/dialog-error.png"));
        emit iconChanged(subscription, QIcon(outPic));
    }
}

void Favicon::fetchIcon(const QUrl &url, const QPixmap &alt) {
    //    qDebug() << Q_FUNC_INFO << "Fetching icon " << url.toString() << " for " << engine->subscription()->toString();
    currentpic = alt;
    blinkAngle = 0;
    QNetworkRequest req(url);
    connect(&nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyReceived(QNetworkReply*)), Qt::UniqueConnection);
    nam.get(req);
    emit iconChanged(subscription, currentpic);
}

void Favicon::replyReceived(QNetworkReply *reply) {
    disconnect(&nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyReceived(QNetworkReply*)));
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QPixmap iconPixmap;
        iconPixmap.loadFromData(bytes);
        if(!iconPixmap.isNull()) {
            currentpic = iconPixmap;
            emit iconChanged(subscription, currentpic);
        }
    }
    reply->deleteLater();
}

void Favicon::update() {
    QPixmap outPic(currentpic);
    blinkAngle -= 0.05;

    QPainter painter(&outPic);

    QRect rect(0, 0, outPic.width(), outPic.height());

    painter.setPen(QColor(255, 255, 255, 64));
    painter.setBrush(QColor(255, 255, 255, 128));
    painter.drawPie(rect, blinkAngle * 5760, 1500);
    painter.setPen(QColor(0, 0, 0, 64));
    painter.setBrush(QColor(0, 0, 0, 128));
    painter.drawPie(rect, blinkAngle * 5760 - (5760/2), 1500);

    emit iconChanged(subscription, QIcon(outPic));
}

/*
void Favicon::engineStateChanged(UpdateEngine *engine, UpdateEngine::UpdateEngineState newState) {
    qDebug() << Q_FUNC_INFO << newState;
    bool shouldBlink = false;
    if(newState==UpdateEngine::PES_UPDATING)
        shouldBlink = true;
    if(engine->subscription() && (engine->subscription()->scheduledForUpdate() || engine->subscription()->beingSynced()))
        shouldBlink = true;


}

void Favicon::engineStateChanged(UpdateEngine::UpdateEngineState newState) {
    UpdateEngine *engine = qobject_cast<UpdateEngine*>(sender());
    engineStateChanged(engine, newState);
}
*/
