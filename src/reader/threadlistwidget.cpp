#include "threadlistwidget.h"

ThreadListWidget::ThreadListWidget(QWidget *parent, ForumDatabase &f) :
	QTreeWidget(parent), fdb(f) {
    setColumnCount(3);
    currentGroup = 0;
    QStringList headers;
    headers << "Subject" << "Date" << "Author" << "Ordernum";
    setHeaderLabels(headers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(&fdb, SIGNAL(messageFound(ForumMessage*)), this, SLOT(messageFound(ForumMessage*)));
    connect(&fdb, SIGNAL(threadFound(ForumThread*)), this, SLOT(threadFound(ForumThread*)));
 //   connect(&fdb, SIGNAL(threadUpdated(ForumThread*)), this, SLOT(threadUpdated(ForumThread*)));
  //  connect(&fdb, SIGNAL(threadDeleted(ForumThread*)), this, SLOT(threadDeleted(ForumThread*)));
  //  connect(&fdb, SIGNAL(messageDeleted(ForumMessage*)), this, SLOT(messageDeleted(ForumMessage*)));
//    connect(&fdb, SIGNAL(messageUpdated(ForumMessage*)), this, SLOT(messageUpdated(ForumMessage*)));
    connect(&fdb, SIGNAL(groupUpdated(ForumGroup*)), this, SLOT(groupUpdated(ForumGroup*)));
    connect(&fdb, SIGNAL(groupDeleted(ForumGroup*)), this, SLOT(groupDeleted(ForumGroup*)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem *)),
            this, SLOT(messageSelected(QTreeWidgetItem*,QTreeWidgetItem *)));
    hideColumn(3);

    markReadAction = new QAction("Mark thread read", this);
    markReadAction->setToolTip("Marks all messages in this thread read");
    connect(markReadAction, SIGNAL(triggered()), this, SLOT(markReadClicked()));
    markUnreadAction = new QAction("Mark thread unread", this);
    markUnreadAction->setToolTip("Marks all messages in this thread as unread");
    connect(markUnreadAction, SIGNAL(triggered()), this, SLOT(markUnreadClicked()));
    threadPropertiesAction = new QAction("Thread properties", this);
    threadPropertiesAction->setToolTip("Information and settings for selected thread");
    connect(threadPropertiesAction, SIGNAL(triggered()), this, SLOT(threadPropertiesClicked()));
    viewInBrowserAction = new QAction("View in browser", this);
    viewInBrowserAction->setToolTip("View the message in external browser");
    connect(viewInBrowserAction, SIGNAL(triggered()), this, SLOT(viewInBrowserClicked()));
}

ThreadListWidget::~ThreadListWidget() {
}

/*
void ThreadListWidget::messageDeleted(ForumMessage *msg) {
    Q_ASSERT(msg);
    if(msg->thread()->group() != currentGroup) return;
    //  qDebug() << Q_FUNC_INFO << msg->toString();

    ThreadListMessageItem *messageItem = forumMessages.key(msg);
    Q_ASSERT(messageItem); // should exist always?
    if(dynamic_cast<ThreadListThreadItem*>(messageItem)) { // Is Thread item
        Q_ASSERT(false); // Never delete the thread item.
    } else {
        messageItem->parent()->removeChild(messageItem);
        forumMessages.remove(messageItem);
    }
    delete messageItem;
}
*/

void ThreadListWidget::groupUpdated(ForumGroup *grp) {
    if(grp != currentGroup) return;

    if(!grp->subscribed()) {
        groupDeleted(grp);
    }
}

void ThreadListWidget::groupDeleted(ForumGroup *grp) {
    if(grp == currentGroup)
        groupSelected(0);
}

void ThreadListWidget::messageFound(ForumMessage *msg) {
    if(msg->thread()->group() == currentGroup) addMessage(msg);
}

void ThreadListWidget::threadFound(ForumThread *thread) {
    if(thread->group() == currentGroup) addThread(thread);
}

void ThreadListWidget::addMessage(ForumMessage *message) {
    Q_ASSERT(message);
    Q_ASSERT(message->thread()->group() == currentGroup);
    // qDebug() << Q_FUNC_INFO << message->toString();
    QPair<QTreeWidgetItem*, ForumThread*> threadPair;

    ThreadListThreadItem *threadItem = forumThreads.key(message->thread());
    Q_ASSERT(threadItem);

    ThreadListMessageItem *item = 0;
    if(message->ordernum() == 0) { // First message - update thread item!
        item = threadItem;
        threadItem->setMessage(message);
        // qDebug() << Q_FUNC_INFO << "setting the thread item";
    } else { // Reply message - create new item
        item = new ThreadListMessageItem(threadItem, message);
    }

    forumMessages[item] = message;
    item->updateItem();
    item->updateRead();

    // Update the thread item
    if(!dynamic_cast<ThreadListThreadItem*> (item)) {
        forumThreads.key(message->thread())->updateUnreads();
    }

    sortItems(3, Qt::AscendingOrder);
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
}

void ThreadListWidget::addThread(ForumThread *thread) {
    Q_ASSERT(thread);
    Q_ASSERT(thread->group() == currentGroup);
    // qDebug() << Q_FUNC_INFO << thread->toString();

    ThreadListThreadItem *threadItem = new ThreadListThreadItem(this, thread);

    forumThreads[threadItem] = thread;
    addTopLevelItem(threadItem);
    sortItems(3, Qt::AscendingOrder);
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
}

void ThreadListWidget::groupSelected(ForumGroup *fg) {
    if(!fg) {
        currentGroup = 0;
        clearList();
        emit messageSelected(0);
    }
    if(currentGroup != fg) {
        currentGroup = fg;
        clearSelection();
        updateList();
    }
}

void ThreadListWidget::clearList() {
    clear();
    forumMessages.clear();
    forumThreads.clear();
}

void ThreadListWidget::updateList() {
    if(!currentGroup) return;
    clearList();

    QList<QTreeWidgetItem *> items;
    foreach(ForumThread *thread, fdb.listThreads(currentGroup)) {
        addThread(thread);
        foreach(ForumMessage *message, fdb.listMessages(thread)) {
            addMessage(message);
        }
    }
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
}
/*
void ThreadListWidget::messageUpdated(ForumMessage *msg) {
    if(msg->thread()->group() != currentGroup) return;

    ThreadListMessageItem *currentMessageItem = forumMessages.key(msg);
    Q_ASSERT(currentMessageItem);
    currentMessageItem->updateItem();
    currentMessageItem->updateRead();
    if(!dynamic_cast<ThreadListThreadItem*> (currentMessageItem)) {
        forumThreads.key(msg->thread())->updateUnreads();
    }
}
*/

void ThreadListWidget::messageSelected(QTreeWidgetItem* item,
                                       QTreeWidgetItem *prev) {
    Q_UNUSED(prev);
    if (!item)
        return;
    if(dynamic_cast<ThreadListShowMoreItem*> (item)) {
        ThreadListShowMoreItem * smItem = dynamic_cast<ThreadListShowMoreItem*> (item);
        ThreadListThreadItem* tli = dynamic_cast<ThreadListThreadItem*> (smItem->parent());
        //tli->deleteShowMore();
        // was: delete show more button. is it needed?
        emit moreMessagesRequested(tli->thread());
    } else if (dynamic_cast<ThreadListMessageItem*> (item)) {
        ThreadListMessageItem* msgItem = dynamic_cast<ThreadListMessageItem*> (item);
        ForumMessage *msg = msgItem->message();
        Q_ASSERT(msg);
        Q_ASSERT(msg->isSane());
        emit messageSelected(msg);
        msgItem->updateRead();
    } else {
        qDebug() << "A thread with no messages. Broken parser?.";
        //if(forumThreads.contains(item))
        //    qDebug() << "The thread in question is " << forumThreads.value(item)->toString();
        emit messageSelected(0);
    }
}


void ThreadListWidget::swapMessages(ForumMessage *m1, ForumMessage *m2) {
    qDebug() << Q_FUNC_INFO << "Swapping " << m1->toString() << " with " << m2->toString();
    /*
    QTreeWidgetItem *i1 = messageWidget(m1);
    QTreeWidgetItem *i2 = messageWidget(m2);

    forumMessages[i1] = m2;
    forumMessages[i2] = m1;
*/
}


void ThreadListWidget::contextMenuEvent(QContextMenuEvent *event) {
    if(itemAt(event->pos())) {
        QMenu menu(this);
        menu.addAction(viewInBrowserAction);
        menu.addAction(markReadAction);
        menu.addAction(markUnreadAction);
        menu.addAction(threadPropertiesAction);
        menu.exec(event->globalPos());
    }
}

void ThreadListWidget::markReadClicked(bool read) {
    ThreadListMessageItem *msgItem = dynamic_cast<ThreadListMessageItem*> (currentItem());
    ForumMessage *threadMessage = msgItem->message();
    if(threadMessage) {
        foreach(ForumMessage *msg, fdb.listMessages(threadMessage->thread())) {
            msg->setRead(read);
        }
    }
}

void ThreadListWidget::markUnreadClicked() {
    markReadClicked(false);
}

void ThreadListWidget::threadPropertiesClicked() {
    ThreadListMessageItem *msgItem = dynamic_cast<ThreadListMessageItem*> (currentItem());
    ForumMessage *threadMessage = msgItem->message();
    if(threadMessage) {
        emit threadProperties(threadMessage->thread());
    }
}

void ThreadListWidget::viewInBrowserClicked() {
    emit viewInBrowser();
}

void ThreadListWidget::selectNextUnread() {
    QTreeWidgetItem *item = currentItem();
    QTreeWidgetItem *newItem = 0;
    ThreadListMessageItem *mi = 0;
    bool isShowMore = false;

    // Find next unread item
    if(item) {
        do {
            if(item->childCount()) {
                // Is a thread item with child
                newItem = item->child(0);
            } else if(item->parent()) {
                // Is a message item
                QTreeWidgetItem *nextItem = item->parent()->child(item->parent()->indexOfChild(item) + 1);
                if(nextItem) {
                    // ..and has a next item after it
                    newItem = nextItem;
                } else {
                    // ..and is last in thread
                    newItem = topLevelItem(indexOfTopLevelItem(item->parent()) + 1);
                }
            } else {
                // Is a thread item without child
                newItem = topLevelItem(indexOfTopLevelItem(item) + 1);
            }
            item = newItem;
            mi = dynamic_cast<ThreadListMessageItem*> (newItem);
            isShowMore = dynamic_cast<ThreadListShowMoreItem*> (newItem);
        } while(isShowMore || (mi && mi->message()->read()));
        if(mi) setCurrentItem(mi);
    }
}
