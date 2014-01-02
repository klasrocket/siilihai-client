#ifndef SubscribeWizard_H
#define SubscribeWizard_H

#include <QtGui>
#include <QObject>
#include <QWizard>
#include <QWidget>
#include <QDebug>

#include <siilihai/forumprobe.h>
#include <siilihai/siilihaiprotocol.h>
#include <siilihai/forumdata/forumsubscription.h>
#include <siilihai/parser/forumparser.h>
#include <siilihai/siilihaisettings.h>
#include "ui_subscribeforum.h"
#include "ui_subscribeforum_login.h"
#include "ui_subscribeforum_verify.h"

class ForumParser;

class SubscribeWizard: public QWizard {
    Q_OBJECT

public:
    SubscribeWizard(QWidget *parent, SiilihaiProtocol &proto, SiilihaiSettings &sett);
    ~SubscribeWizard();
    QWizardPage *createIntroPage();
    QWizardPage *createLoginPage();
    QWizardPage *createVerifyPage();
signals:
    void forumAdded(ForumSubscription *fs); //fs valid during signal call

private slots:
    void listForumsFinished(QList <ForumSubscription*> parsers);
    void updateForumList();
    void pageChanged(int id);
    void wizardAccepted();
    void getParserFinished(ForumParser *parser);
    void comboItemChanged(int id);
    void forumClicked(QListWidgetItem* newItem);
    void newForumAdded(ForumSubscription *sub);
    void probeResults(ForumSubscription *probedSub);
    void forumGot(ForumSubscription *sub);
    void checkUrlValidity();
private:
    SiilihaiProtocol &protocol;
    SiilihaiSettings &settings;
    Ui::SubscribeForm subscribeForm;
    Ui::SubscribeForumLoginForm subscribeForumLogin;
    Ui::SubscribeForumVerify subscribeForumVerify;
    QList <ForumSubscription*> allForums;
    QHash <QListWidgetItem*, ForumSubscription*> listWidgetItemForum;
    ForumSubscription *selectedForum;
    ForumParser parser;
    ForumSubscription newForum;
    ForumProbe probe;
};

#endif // SubscribeWizard_H
