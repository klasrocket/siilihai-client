#ifndef FORUMLISTWIDGET_H
#define FORUMLISTWIDGET_H

// Qt5
#include <QWidget>
#include <QToolBox>
#include <QHash>
#include <QListWidgetItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>

#include <siilihai/parser/parserengine.h>

class Favicon;
class ForumGroup;
class ForumSubscription;

class ForumListWidget: public QToolBox {
    Q_OBJECT

public:
    ForumListWidget(QWidget *parent);
    ~ForumListWidget();
    void updateReadCounts(ForumGroup *grp);
    ForumSubscription *getSelectedForum();
    ForumGroup *getSelectedGroup();
public slots:
    void groupSelected(QListWidgetItem* item, QListWidgetItem *prev);
    void forumItemSelected(int i);
    void iconUpdated(ForumSubscription* en, QIcon newIcon);
    void unsubscribeGroupClicked();
    void markAllReadClicked(bool un=false);
    void markAllUnreadClicked();
    void groupSubscriptionsClicked();
    void updateSubscriptionLabel();
    void updateSubscriptionLabel(ForumSubscription *sub);
    void updateGroupLabel();
    void updateGroupLabel(ForumGroup *grp);
    void addSubscription(ForumSubscription *sub); // subscription's parser MUST be set!
private slots:
    void subscriptionChanged(ForumSubscription *sub);
    void subscriptionDeleted(ForumSubscription *sub);
    void subscriptionDeleted(QObject *qo);
    void groupFound(ForumGroup *grp);
    void groupDeleted(ForumGroup *grp);
    void groupDestroyed(QObject*);
    void groupChanged();
    void groupChanged(ForumGroup *grp);
    void parserEngineStateChanged(UpdateEngine::UpdateEngineState state);
signals:
    void groupSelected(ForumGroup *grp);
    void forumSelected(ForumSubscription *sub);
    void unsubscribeGroup(ForumGroup *grp);
    void groupSubscriptions(ForumSubscription *sub);
    void unsubscribeForum();
    void forumProperties();
    void groupUnselected(ForumGroup *group); // and could now be synced etc
    void displaySubscriptionErrors(ForumSubscription *sub);
protected:
    void contextMenuEvent(QContextMenuEvent * event);
private:
    void setupFavicon(ForumSubscription *sub);
    ForumGroup *currentGroup;
    QMap<ForumSubscription*, Favicon*> forumIcons;
    QMap<QListWidgetItem*, ForumGroup*> forumGroups;
    QMap<ForumSubscription*, QListWidget*> listWidgets;
    // Actions:
    QAction *unsubscribeAction;
    QAction *markReadAction;
    QAction *markUnreadAction;
    QAction *groupSubscriptionsAction;
    QAction *unsubscribeForumAction;
    QAction *forumPropertiesAction;
};

#endif // FORUMLISTWIDGET_H
