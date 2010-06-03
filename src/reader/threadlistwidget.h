#ifndef THREADLISTWIDGET_H_
#define THREADLISTWIDGET_H_

#include <QTreeWidget>
#include <QList>
#include <QPair>
#include <QMenu>
#include <QContextMenuEvent>

#include <siilihai/forumgroup.h>
#include <siilihai/forumthread.h>
#include <siilihai/forummessage.h>
#include <siilihai/forumdatabase.h>

#include "messageformatting.h"

class ThreadListWidget : public QTreeWidget {
	Q_OBJECT

public:
	ThreadListWidget(QWidget *parent, ForumDatabase &f);
	virtual ~ThreadListWidget();
public slots:
	void groupSelected(ForumGroup *fg);
	void updateMessageRead(QTreeWidgetItem *item);
        void updateMessageItem(QTreeWidgetItem *item, ForumMessage *msg);
        void messageSelected(QTreeWidgetItem* item, QTreeWidgetItem *prev);
        void messageFound(ForumMessage *msg);
        void messageUpdated(ForumMessage *msg);
        void messageDeleted(ForumMessage *msg);
        void threadFound(ForumThread *thread);
        void threadUpdated(ForumThread *thread);
        void threadDeleted(ForumThread *thread);
        void groupUpdated(ForumGroup *grp);
        void groupDeleted(ForumGroup *grp);
        void markReadClicked(bool read=true);
        void markUnreadClicked();
        void threadPropertiesClicked();
        void viewInBrowserClicked();
signals:
	void messageSelected(ForumMessage *msg);
        void moreMessagesRequested(ForumThread *thread);
        void viewInBrowser();
        void threadProperties(ForumThread *thread);
protected:
        void contextMenuEvent(QContextMenuEvent * event);

private:
        QString messageSubject(ForumMessage *msg);
        void swapMessages(ForumMessage *m1, ForumMessage *m2);
        void updateList();
        void clearList();
        void addThread(ForumThread *thread);
        void addMessage(ForumMessage *msg);
	void updateThreadUnreads(QTreeWidgetItem* threadItem);
        void addShowMoreButton(ForumThread *thread);
        QTreeWidgetItem* messageWidget(ForumMessage *msg);
        QTreeWidgetItem* threadWidget(ForumThread *thread);
        QHash<QTreeWidgetItem*, ForumThread*> showMoreItems;
        QHash<QTreeWidgetItem*, ForumMessage*> forumMessages;
        QHash<QTreeWidgetItem*, ForumThread*> forumThreads;
        QHash<QTreeWidgetItem*, QString> messageSubjects;
        ForumGroup *currentGroup;
	ForumDatabase &fdb;
        // Actions:
        QAction *markReadAction;
        QAction *markUnreadAction;
        QAction *threadPropertiesAction;
        QAction *viewInBrowserAction;
};

#endif /* THREADLISTWIDGET_H_ */
