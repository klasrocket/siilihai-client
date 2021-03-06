#ifndef PARSERMAKER_H
#define PARSERMAKER_H

#include <QMainWindow>
#include <QSettings>
#include <siilihai/siilihaiprotocol.h>
#include <siilihai/parser/forumsubscriptionparsed.h>
#include <siilihai/siilihaisettings.h>
#include "downloaddialog.h"
#include "openrequestdialog.h"
#include "patterneditor.h"
#include "threadlistpatterneditor.h"
#include "grouplistpatterneditor.h"
#include "messagelistpatterneditor.h"
#include "../common/credentialsdialog.h"

#include "ui_parsermaker.h"

class ParserManager;
class ForumRequest;

class ParserMaker : public QMainWindow
{
    Q_OBJECT

public:
    ParserMaker(QWidget *parent, ParserManager *pd, SiilihaiSettings &s, SiilihaiProtocol &p);
    ~ParserMaker();

private slots:
    void updateState();
    void openClicked();
    void newFromRequestClicked();
    void saveClicked();
    void saveAsNewClicked();
    void testForumUrlClicked();
    void parserLoaded(ForumParser *p);
    void saveParserFinished(int newId, QString message);
    void requestSelected(ForumRequest *req);
    void tryLogin();
    void tryWithoutLogin();
    void loginFinished(ForumSubscription *sub, bool success);
    void helpClicked();
    void getHttpAuthentication(ForumSubscription *, QAuthenticator *);
    void dataMatchingStart(QString &html);
    void dataMatchingEnd();
    void dataMatched(int pos, QString data, PatternMatchType type);
    void subscriptionErrorsChanged();

signals:
    void parserSaved(ForumParser *savedParser);

private:
    void closeEvent(QCloseEvent *event);
    Ui::ParserMakerWindow ui;
    ParserManager *pdb;
    SiilihaiSettings &settings;
    SiilihaiProtocol &protocol;
    ForumParser parser;
    QNetworkAccessManager nam;
    ParserEngine engine;
    ForumSubscriptionParsed subscription;
    PatternEditor *groupListEditor, *threadListEditor, *messageListEditor;
    ForumGroup *selectedGroup;
    ForumThread *selectedThread;
    bool loginWithoutCredentials;
    PatternMatcher *loginMatcher;
    QTextCursor loginEditorCursor;
};

#endif // PARSERMAKER_H
