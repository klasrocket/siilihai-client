#include "forumlistwidget.h"

ForumListWidget::ForumListWidget(QWidget *parent, ForumDatabase &f,
                                 ParserDatabase &p) :
QToolBox(parent), fdb(f), pdb(p) {
    connect(this, SIGNAL(currentChanged(int)), this,
            SLOT(forumItemSelected(int)));
    connect(&f, SIGNAL(subscriptionFound(ForumSubscription *)), this, SLOT(subscriptionFound(ForumSubscription *)));
    connect(&f, SIGNAL(groupFound(ForumGroup *)), this, SLOT(groupFound(ForumGroup *)));
    connect(&f, SIGNAL(groupUpdated(ForumGroup *)), this, SLOT(groupUpdated(ForumGroup *)));
    connect(&f, SIGNAL(groupDeleted(ForumGroup *)), this, SLOT(groupDeleted(ForumGroup *)));
    connect(&f, SIGNAL(subscriptionDeleted(ForumSubscription*)), this, SLOT(subscriptionDeleted(ForumSubscription*)));
    connect(&f, SIGNAL(messageUpdated(ForumMessage*)), this, SLOT(messageUpdated(ForumMessage*)));

    markReadAction = new QAction("Mark all messages read", this);
    markReadAction->setToolTip("Mark all messages in selected group as read");
    connect(markReadAction, SIGNAL(triggered()), this, SLOT(markAllReadClicked()));
    markUnreadAction = new QAction("Mark all messages unread", this);
    markUnreadAction->setToolTip("Mark all messages in selected group as unread");
    connect(markUnreadAction, SIGNAL(triggered()), this, SLOT(markAllUnreadClicked()));
    unsubscribeAction = new QAction("Unsubscribe group..", this);
    unsubscribeAction->setToolTip("Unsubscribe from the selected group");
    connect(unsubscribeAction, SIGNAL(triggered()), this, SLOT(unsubscribeGroupClicked()));
    groupSubscriptionsAction = new QAction("Group subscriptions..", this);
    groupSubscriptionsAction->setToolTip("Subscribe and unsubscribe to groups in this forum");
    connect(groupSubscriptionsAction, SIGNAL(triggered()), this, SLOT(groupSubscriptionsClicked()));
    unsubscribeForumAction = new QAction("Unsubscribe forum..",this);
    unsubscribeForumAction->setToolTip("Unsubscribe from the forum");
    connect(unsubscribeForumAction, SIGNAL(triggered()), this, SIGNAL(unsubscribeForum()));
    forumPropertiesAction = new QAction("Forum properties..", this);
    forumPropertiesAction->setToolTip("View and edit detailed information about the forum");
    connect(forumPropertiesAction, SIGNAL(triggered()), this, SIGNAL(forumProperties()));
}

ForumListWidget::~ForumListWidget() {

}

void ForumListWidget::forumItemSelected(int i) {
    qDebug() << Q_FUNC_INFO << " selected forum index " << i;
    ForumSubscription *sub = 0;
    if(i != -1) {
        QHashIterator<ForumSubscription*, QListWidget*> it(forumSubscriptions);
        while (it.hasNext()) {
            it.next();
            if(indexOf(it.value()) == i) {
                sub = it.key();
            }
        }
        Q_ASSERT(sub);
    }

    foreach(ForumGroup *grp, fdb.listGroups(sub))
        groupUpdated(grp);

    emit forumSelected(sub);
}

void ForumListWidget::messageUpdated(ForumMessage *msg) {
    groupUpdated(msg->thread()->group());
}

ForumSubscription* ForumListWidget::getSelectedForum() {
    ForumSubscription *sub = 0;
    QListWidget *curWidget = static_cast<QListWidget*> (currentWidget());
    if(curWidget) {
        sub = forumSubscriptions.key(curWidget);
        Q_ASSERT(sub);
    }
    return sub;
}

ForumGroup* ForumListWidget::getSelectedGroup() {
    QListWidget *lw = static_cast<QListWidget*> (currentWidget());
    if (lw) {
        QListWidgetItem* it = lw->currentItem();
        if (it) {
            return forumGroups[it];
        }
    }
    return 0;
}

void ForumListWidget::iconUpdated(ForumSubscription *forum, QIcon newIcon) {
    Q_ASSERT(forumSubscriptions.contains(forum));
    setItemIcon(indexOf(forumSubscriptions[forum]), newIcon);
}

void ForumListWidget::setForumStatus(ForumSubscription* forum, bool reloading, float progress) {
    Q_ASSERT(forumSubscriptions.contains(forum));
    Favicon *fi = forumIcons[forum];

    fi->setReloading(reloading, progress);
}

void ForumListWidget::groupSelected(QListWidgetItem* item,
                                    QListWidgetItem *prev) {
    qDebug() << Q_FUNC_INFO << " selected item " << item << ", prev " << prev;
    currentGroup = forumGroups[item];
    emit groupSelected(currentGroup);
}

void ForumListWidget::subscriptionFound(ForumSubscription *sub) {
    qDebug() << Q_FUNC_INFO << sub << currentIndex();
    Q_ASSERT(sub);
    QListWidget *lw = new QListWidget(this);
    forumSubscriptions[sub] = lw;
    addItem(lw, sub->alias());
    connect(lw, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem *)),
            this, SLOT(groupSelected(QListWidgetItem*,QListWidgetItem *)));

    QString fiUrl = pdb.getParser(sub->parser()).forum_url;
    fiUrl = fiUrl.replace(QUrl(fiUrl).path(), "");
    fiUrl = fiUrl + "/favicon.ico";
    Favicon *fi = new Favicon(this, sub);
    connect(fi, SIGNAL(iconChanged(ForumSubscription*, QIcon)), this,
            SLOT(iconUpdated(ForumSubscription*, QIcon)));
    fi->fetchIcon(QUrl(QUrl(fiUrl)), QPixmap(":/data/emblem-web.png"));
    forumIcons[sub] = fi;
}

void ForumListWidget::groupFound(ForumGroup *grp) {
    Q_ASSERT(grp);
    qDebug() << Q_FUNC_INFO << grp->toString();
    if(!grp->subscribed()) return;
    Q_ASSERT(forumSubscriptions.contains(grp->subscription()));
    QListWidget *lw = forumSubscriptions[grp->subscription()];
    Q_ASSERT(lw);
    QListWidgetItem *lwi = new QListWidgetItem(lw);
    lwi->setIcon(QIcon(":/data/folder.png"));
    lw->addItem(lwi);
    forumGroups[lwi] = grp;
    groupUpdated(grp);
}


void ForumListWidget::groupUpdated(ForumGroup *grp) {
    Q_ASSERT(grp);
    QListWidget *lw = forumSubscriptions[grp->subscription()];
    Q_ASSERT(lw);
    QListWidgetItem *gItem = groupItem(grp);
    if(gItem && !grp->subscribed()) {
        qDebug() << Q_FUNC_INFO << " deleting unsubscribed group from UI";
        lw->takeItem(lw->row(gItem));
        forumGroups.remove(gItem);
        delete gItem;
        if(currentGroup == grp) {
            currentGroup = 0;
            emit groupSelected(currentGroup);
        }
    } else if(gItem && grp->subscribed()) {
        int unread = fdb.unreadIn(grp);
        QString title = grp->name();
        if (unread > 0)
            title = title + " (" + QString().number(unread) + ")";
        qDebug() << Q_FUNC_INFO << " Updating group title to " << title;
        gItem->setText(title);
    } else if(!gItem && grp->subscribed()) {
        qDebug() << Q_FUNC_INFO << " adding subscribed group to UI";
        groupFound(grp);
    }
}

void ForumListWidget::groupDeleted(ForumGroup *grp) {
    if(!grp->subscribed()) return;
    if(currentGroup==grp) {
        currentGroup = 0;
        emit groupSelected(0);
    }
    QListWidgetItem *item = groupItem(grp);
    Q_ASSERT(item);
    QListWidget *lw = forumSubscriptions[grp->subscription()];
    Q_ASSERT(lw);
    lw->removeItemWidget(item);
    forumGroups.remove(item);
    delete item;
}

void ForumListWidget::subscriptionDeleted(ForumSubscription *sub) {
    if(currentGroup && currentGroup->subscription()==sub) {
        currentGroup = 0;
        emit forumSelected(0);
    }
    QListWidget *lw = forumSubscriptions[sub];
    Q_ASSERT(lw);
    removeItem(indexOf(lw));
    forumSubscriptions.remove(sub);
    lw->deleteLater();
}

QListWidgetItem * ForumListWidget::groupItem(ForumGroup *grp) {
    foreach(QListWidgetItem *lwi, forumGroups.keys()) {
        if(forumGroups[lwi] == grp) {
            return lwi;
        }
    }
    return 0;
}

void ForumListWidget::contextMenuEvent(QContextMenuEvent *event) {
    QListWidget *lw = qobject_cast<QListWidget*>(currentWidget());

    // @todo could use lw->itemAt(event->pos()) to detect when
    // not clicking on item.
    if(lw) {
        QMenu menu(this);
        if(lw->itemAt(lw->mapFromGlobal(event->globalPos()))) {
            menu.addAction(markReadAction);
            menu.addAction(markUnreadAction);
            menu.addSeparator();
            menu.addAction(unsubscribeAction);
        }
        menu.addAction(groupSubscriptionsAction);
        menu.addSeparator();
        menu.addAction(forumPropertiesAction);
        menu.addAction(unsubscribeForumAction);
        menu.exec(event->globalPos());
    }
}

void ForumListWidget::unsubscribeGroupClicked() {
    if(currentGroup) {
        QMessageBox msgBox;
        msgBox.setText("Really unsubscribe from group?");
        msgBox.setInformativeText(currentGroup->name());
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec()==QMessageBox::Yes)
            emit unsubscribeGroup(currentGroup);
    }
}

void ForumListWidget::markAllReadClicked(bool un) {
    if(currentGroup) {
        foreach(ForumThread *thread, fdb.listThreads(currentGroup)) {
            foreach(ForumMessage *msg, fdb.listMessages(thread)) {
                if(msg->read() == un) {
                    fdb.markMessageRead(msg, !un);
                    QCoreApplication::processEvents();
                }
            }
        }
    }
}

void ForumListWidget::markAllUnreadClicked() {
    markAllReadClicked(true);
}

void ForumListWidget::groupSubscriptionsClicked() {
    if(getSelectedForum()) emit groupSubscriptions(getSelectedForum());
}
