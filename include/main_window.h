/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QItemSelection>
#include <QKeySequence>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QProgressBar>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSystemTrayIcon>
#include <QTimer>
#include <array>

#include "app_settings.h"
#include "crypto.h"
#include "identicon.h"
#include "import_export.h"
#include "uitemplate_helpers.h"
#include "user.h"
#ifndef DISABLE_FILL_FORM_SHORTCUTS
#include "keypress.h"
#endif

namespace Ui {
class MainWindow;
}
class MainWindow;

class MySortFilterProxyModel : public QSortFilterProxyModel {
   public:
    MySortFilterProxyModel(MainWindow& main_window, QObject* parent = 0)
        : QSortFilterProxyModel(parent), m_main_window(main_window)
    {
    }

   protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

   private:
    MainWindow& m_main_window;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    enum class ShortcutAction {
        CopyPWToClipboard,
        CopyUserToClipboard,
#ifndef DISABLE_FILL_FORM_SHORTCUTS
        FillForm,
        FillFormPasswordOnly,
#endif
        SelectFilter,
        PreviousItem,
        NextItem,
        OpenURL,
        Logout,

        Count
    };
    static QString description(ShortcutAction action);
    const vector<QKeySequence>& shortcuts(ShortcutAction action) const
    {
        return m_table_shortcuts[(int)action];
    }

    CategoryId selectedCategory() const { return m_selected_category; }
    QStandardItemModel* model() const { return m_sites_model; }
    bool trayIconEnabled() const { return m_application_settings.show_systray_icon; }

   private:
    Ui::MainWindow* m_ui;
    void initSitesView();
    void initTrayIcon();
    void addSiteToUI(UiSite& site);
    void clearSitesUI();
    TableItem* getSelectedItem();
    QModelIndex getSelectedRow();
    void updateModelItem(int row, const UiSite& site);
    void addCategory(const QString& name, CategoryId id = -1);
    void removeCategory(CategoryId category_id);
    void selectCategory(CategoryId category);
    void uiSitesTableChanged();
    void copyPWToClipboard(const UiSite& site);
    void copyUserToClipboard(const UiSite& site);
    int copyToClipboard(const QString& str);
    void activateLogoutTimer();
    void abortLogoutTimer();
    void openSelectedUrl();
#ifndef DISABLE_FILL_FORM_SHORTCUTS
    void fillForm(bool password_only = false);
#endif

    MasterPassword m_master_password;
    Identicon m_identicon;
    QTimer* m_hide_identicon_timer = nullptr;
    QTimer* m_delayed_identicon_update_timer = nullptr;
    QMap<QString /* user name */, UiUser> m_users;
    QMap<CategoryId, QString> m_categories;
    CategoryId m_next_category_id = 0;
    CategoryId m_selected_category = -1;
    UiUser* m_current_user = nullptr; /** current logged in user */
    QTimer* m_logout_timer = nullptr;

    QStandardItemModel* m_sites_model;
    MySortFilterProxyModel* m_proxy_model;

    QSystemTrayIcon* m_tray_icon = nullptr;
    QAction* m_tray_icon_show_action = nullptr;
    ApplicationSettings m_application_settings;
    DataImportExport m_import_export;

    QString m_clipboard_data;
    QString m_clipboard_previous_data;
    QTimer* m_clipboard_timer = nullptr;
    QProgressBar* m_status_progress_bar = nullptr;
    int m_clipboard_time_left;

    int m_copy_column_idx;

#ifndef DISABLE_FILL_FORM_SHORTCUTS
    Keypress m_keypress;
#endif

    /** key bindings for actions on selected item. an action can have multiple
     *  shortcuts */
    std::array<vector<QKeySequence>, (int)ShortcutAction::Count> m_table_shortcuts;

   private slots:
    void login();
    void logout();
    void loginLogoutClicked();
    void addUser();
    void editUser();
    void deleteUser();
    void addSite();
    void deleteSite();
    void editSite();
    void saveSettings();
    void readSettings();
    void enableUI(bool logged_in);
    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void categoryButtonPressed();
    void filterTextChanged(QString filter_text);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void iconAboutToShow();
    void showTrayIcon(bool visible);
    void showSettingsWidget();
    void quit();
    void showAboutWidget();
    void showShortcutsWidget();
    void clearDataFromClipboard();
    void clearDataFromClipboardTimer();
    void appAboutToQuit();
    void uiLoginChanged();
    void showGeneratePasswordDialog();
    void delayedIdenticonUpdate();
   public slots:
    void showHide();
    void copyPWToClipboardClicked();
    void showHidePWClicked();
    void openUrlClicked();

   protected:
    void closeEvent(QCloseEvent* event);
    bool eventFilter(QObject* obj, QEvent* event);
};

class DBusAdapter : public QObject {
    Q_OBJECT
   public:
    DBusAdapter(MainWindow* main_window);

   private:
    MainWindow* m_main_window;
   public slots:
    Q_SCRIPTABLE void showHide();
};

#endif  // MAIN_WINDOW_H
