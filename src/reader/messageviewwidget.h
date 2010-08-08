#ifndef MESSAGEVIEWWIDGET_H_
#define MESSAGEVIEWWIDGET_H_
#include <QObject>
#include <QScrollArea>
#include <QWebView>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QDir>

#include <siilihai/forummessage.h>
#include "messageformatting.h"

class MessageViewWidget : public QScrollArea {
    Q_OBJECT

public:
    MessageViewWidget(QWidget *parent);
    virtual ~MessageViewWidget();
    ForumMessage* currentMessage();
    bool scrollDown(); // Scrolls down if possible and returns true, if it did so.
public slots:
    void messageSelected(ForumMessage *msg);
    void linkClicked ( const QUrl & url);
private:
    QWebView webView;
    QVBoxLayout vbox;
    ForumMessage *displayedMessage;
};

#endif /* MESSAGEVIEWWIDGET_H_ */
