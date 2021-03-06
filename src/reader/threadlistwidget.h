#ifndef THREADLISTWIDGET_H_
#define THREADLISTWIDGET_H_

#include <QTreeWidget>
#include <QList>
#include <QPair>
#include <QMenu>
#include <QContextMenuEvent>
#include <QObject>
#include <QMessageBox>

#include "threadlistmessageitem.h"
#include "threadlistthreaditem.h"
#include "threadlistshowmoreitem.h"

class ForumGroup;
class ForumThread;
class ForumMessage;

class ThreadListWidget : public QTreeWidget {
    Q_OBJECT

public:
    ThreadListWidget(QWidget *parent);
    virtual ~ThreadListWidget();
public slots:
    void groupSelected(ForumGroup *fg);
    void selectNextUnread();

private slots:
    void messageSelected(QTreeWidgetItem* item, QTreeWidgetItem *prev);
    void groupChanged();
    void groupDeleted(QObject*);
    void markReadClicked(bool read=true);
    void markUnreadClicked();
    void threadPropertiesClicked();
    void viewInBrowserClicked();
    void forceUpdateThreadClicked();

private slots:
    void addThread(ForumThread *thread);
    void sortColumns();
signals:
    void messageSelected(ForumMessage *msg);
    void moreMessagesRequested(ForumThread *thread);
    void viewInBrowser();
    void threadProperties(ForumThread *thread);
    void updateThread(ForumThread *thread, bool force);
protected:
    void contextMenuEvent(QContextMenuEvent * event);

private:
    QString messageSubject(ForumMessage *msg);
    void updateList();
    void clearList();

    void updateThreadUnreads(ThreadListThreadItem* threadItem);
    void addShowMoreButton(ForumThread *thread);

    ForumGroup *currentGroup;
    // Actions:
    QAction *markReadAction;
    QAction *markUnreadAction;
    QAction *threadPropertiesAction;
    QAction *viewInBrowserAction;
    QAction *forceUpdateThreadAction;
    bool disableSortAndResize; // Don't sort or resize while doing long add
};

#endif /* THREADLISTWIDGET_H_ */
