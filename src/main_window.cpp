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

#include "main_window.h"
#include "ui_main_window.h"
#include "user_widget.h"
#include "edit_site_widget.h"
#include "logging.h"
#include "pushbutton_delegate.h"
#include "settings_widget.h"
#include "shortcuts_widget.h"
#include "password_generator_widget.h"
#include "version.h"

#include <iostream>
using namespace std;

#include <QMessageBox>
#include <QSettings>
#include <QDataStream>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QClipboard>
#include <QCloseEvent>
#include <QtGlobal>
#include <QDesktopServices>
#include <QUrl>
#include <QThread>
#include <QRegularExpression>

#ifdef Q_OS_LINUX
#include <QtDBus/QtDBus>
#endif

DBusAdapter::DBusAdapter(MainWindow* main_window) :
		QObject(main_window), m_main_window(main_window) {
}
void DBusAdapter::showHide() {
	m_main_window->showHide();
	if (m_main_window->isVisible()) {
		m_main_window->activateWindow();
		m_main_window->raise();
	}
}

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent), m_ui(new Ui::MainWindow),
		m_import_export(m_categories) {
	m_ui->setupUi(this);

	m_logout_timer = new QTimer(this);
	connect(m_logout_timer, SIGNAL(timeout()), this, SLOT(logout()));
	m_logout_timer->setSingleShot(true);

	m_clipboard_timer = new QTimer(this);
	connect(m_clipboard_timer, SIGNAL(timeout()), this,
			SLOT(clearDataFromClipboardTimer()));
	m_hide_identicon_timer = new QTimer(this);
	connect(m_hide_identicon_timer, SIGNAL(timeout()), m_ui->lblIdenticon, SLOT(clear()));
	m_hide_identicon_timer->setSingleShot(true);

	m_delayed_identicon_update_timer = new QTimer(this);
	connect(m_delayed_identicon_update_timer, SIGNAL(timeout()), this, SLOT(delayedIdenticonUpdate()));
	m_delayed_identicon_update_timer->setSingleShot(true);

	initSitesView();
	readSettings();
	m_ui->btnDeleteUser->setEnabled(m_ui->cmbUserName->count() > 0);
	m_ui->btnEditUser->setEnabled(m_ui->cmbUserName->count() > 0);
	enableUI(false);
	showTrayIcon(m_application_settings.show_systray_icon);
	setWindowIcon(QIcon(":/app_icon.png"));
	connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()),
			this, SLOT(appAboutToQuit()));

	m_status_progress_bar = new QProgressBar(statusBar());
	m_status_progress_bar->setTextVisible(false);
	m_status_progress_bar->setMinimum(0);
	m_status_progress_bar->setSizePolicy(QSizePolicy::Minimum,
			m_status_progress_bar->sizePolicy().verticalPolicy());
	m_status_progress_bar->hide();
	statusBar()->addPermanentWidget(m_status_progress_bar);

	QFont label_font = m_ui->lblIdenticon->font();
	label_font.setPointSizeF(label_font.pointSizeF()*1.3f);
	m_ui->lblIdenticon->setFont(label_font);

	/* default key bindings */
	m_table_shortcuts[(int)ShortcutAction::CopyPWToClipboard].push_back(
			QKeySequence(QKeySequence::Copy));
	m_table_shortcuts[(int)ShortcutAction::CopyPWToClipboard].push_back(
			QKeySequence(Qt::Key_Space));
	m_table_shortcuts[(int)ShortcutAction::CopyPWToClipboard].push_back(
			QKeySequence(Qt::Key_Y));
	m_table_shortcuts[(int)ShortcutAction::CopyUserToClipboard].push_back(
			QKeySequence(Qt::Key_U));
	m_table_shortcuts[(int)ShortcutAction::FillForm].push_back(
			QKeySequence(QKeySequence::Paste));
	m_table_shortcuts[(int)ShortcutAction::FillForm].push_back(
			QKeySequence(Qt::Key_P));
	m_table_shortcuts[(int)ShortcutAction::FillFormPasswordOnly].push_back(
			QKeySequence(Qt::SHIFT | Qt::Key_P));
	m_table_shortcuts[(int)ShortcutAction::SelectFilter].push_back(
			QKeySequence(Qt::Key_Slash));
	m_table_shortcuts[(int)ShortcutAction::PreviousItem].push_back(
			QKeySequence(Qt::Key_K));
	m_table_shortcuts[(int)ShortcutAction::NextItem].push_back(
			QKeySequence(Qt::Key_J));
	m_table_shortcuts[(int)ShortcutAction::OpenURL].push_back(
			QKeySequence(Qt::Key_O));
	m_table_shortcuts[(int)ShortcutAction::Logout].push_back(
			QKeySequence(Qt::Key_Q));

#ifdef Q_OS_LINUX
	if (!QDBusConnection::sessionBus().isConnected()) {
		LOG(LogLevel::Warn, "Cannot connect to the D-Bus session bus.\n"
				"To start it, run: eval `dbus-launch --auto-syntax`\n");
	} else if (!QDBusConnection::sessionBus().registerService(
			"org.bkueng.qMasterPassword")) {
		LOG(LogLevel::Warn, "%s", qPrintable(QDBusConnection::sessionBus().lastError().message()));
	} else {
		DBusAdapter* dbus_adapter = new DBusAdapter(this);
		QDBusConnection::sessionBus().registerObject("/MainWindow", dbus_adapter,
				QDBusConnection::ExportAllSlots);
	}
#endif /* Q_OS_LINUX */
}

MainWindow::~MainWindow() {
	delete m_ui;
}

void MainWindow::initSitesView() {
	m_sites_model = new QStandardItemModel(this);
	m_sites_model->setColumnCount(7);
	int i = 0;
	m_sites_model->setHeaderData(i++, Qt::Horizontal, QObject::tr("Site"));
	m_sites_model->setHeaderData(i++, Qt::Horizontal, QObject::tr("Login Name"));
	m_sites_model->setHeaderData(i++, Qt::Horizontal, QObject::tr("Password"));
	m_sites_model->setHeaderData(m_copy_column_idx = i++, Qt::Horizontal,
			QObject::tr("")); //copy pw button
	m_sites_model->setHeaderData(i++, Qt::Horizontal, QObject::tr("Comment"));
	m_sites_model->setHeaderData(i++, Qt::Horizontal, QObject::tr("Type"));
	m_sites_model->setHeaderData(i++, Qt::Horizontal, QObject::tr("Counter"));

	m_proxy_model = new MySortFilterProxyModel(*this, this);
	m_proxy_model->setSourceModel(m_sites_model);
	m_ui->tblSites->setModel(m_proxy_model);

	PushButtonDelegate* button_item_delegate = new PushButtonDelegate(*this, this);
	m_ui->tblSites->setItemDelegateForColumn(m_copy_column_idx, button_item_delegate);

	m_ui->tblSites->horizontalHeader()->setSectionResizeMode(m_copy_column_idx,
		QHeaderView::Fixed);

    connect(m_ui->tblSites->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this,
            SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
	m_ui->tblSites->installEventFilter(this);
}

void MainWindow::initTrayIcon() {
	if (m_tray_icon) return;

	m_tray_icon_show_action = new QAction("", this);
	connect(m_tray_icon_show_action, SIGNAL(triggered()), this, SLOT(showHide()));
	QAction* quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	QMenu* trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(m_tray_icon_show_action);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	m_tray_icon = new QSystemTrayIcon(this);
	m_tray_icon->setContextMenu(trayIconMenu);
	m_tray_icon->setIcon(QIcon(":/app_icon.png"));
	m_tray_icon->setToolTip("qMasterPassword");

	connect(trayIconMenu, SIGNAL(aboutToShow()), this, SLOT(iconAboutToShow()));

	connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::addSiteToUI(UiSite& site) {
	QList<QStandardItem*> items;
	for (int i = 0; i < 7; ++i) {
		QStandardItem* item = new TableItem(site, "");
		item->setEditable(false);
		item->setDragEnabled(false);
		item->setDropEnabled(false);
		items.push_back(item);
	}
	m_sites_model->appendRow(items);
	updateModelItem(m_sites_model->rowCount()-1, site);

	/*
	//simpler version to display a 'copy to clipboard' button
	// (but it will be hidden after filter changes)
	QPushButton* btn = new UserPushButton(site, "");
	btn->setIcon(QIcon(":/copy.png"));
	btn->setFixedWidth(btn->sizeHint().width());
	connect(btn, SIGNAL(clicked()), this, SLOT(copyPWToClipboardClicked()));
	m_ui->tblSites->setIndexWidget(m_proxy_model->mapFromSource(
		m_sites_model->index(m_sites_model->rowCount() - 1, m_copy_column_idx)), btn);
	*/

	uiSitesTableChanged();
}
void MainWindow::updateModelItem(int row, const UiSite& site) {
	int i = 0;
	m_sites_model->item(row, i++)->setText(QString::fromUtf8(site.site.getName().c_str()));
	m_sites_model->item(row, i++)->setText(site.user_name);
	if (site.password_visible) {
		string password = m_master_password.sitePassword(site.site);
		m_sites_model->item(row, i++)->setText(QString::fromUtf8(password.c_str()));
	} else {
		m_sites_model->item(row, i++)->setText("***");
	}
	//update the buttons
	m_ui->tblSites->closePersistentEditor(m_proxy_model->mapFromSource(
			m_sites_model->index(row, m_copy_column_idx)));
	m_ui->tblSites->openPersistentEditor(m_proxy_model->mapFromSource(
			m_sites_model->index(row, m_copy_column_idx)));
	++i; //copy button
	m_sites_model->item(row, i++)->setText(site.comment);
	m_sites_model->item(row, i++)->setText(QString::fromUtf8(
			MPSiteTypeToString(site.site.getType()).c_str()));
	m_sites_model->item(row, i++)->setText(QString::number(site.site.getCounter()));
}

void MainWindow::clearSitesUI() {
	m_sites_model->removeRows(0, m_sites_model->rowCount());
	m_ui->btnDeleteSite->setEnabled(false);
	m_ui->btnEditSite->setEnabled(false);
}

void MainWindow::loginLogoutClicked() {
	m_delayed_identicon_update_timer->stop();
	if (m_master_password.isLoggedIn()) {
		logout();
	} else {
		delayedIdenticonUpdate();
		login();
	}
}

void MainWindow::login() {
	LOG(LogLevel::Debug, "Login");
	QString user_name = m_ui->cmbUserName->currentText();
	QString password = m_ui->txtPassword->text();
	auto iter_user = m_users.find(user_name);
	if (user_name.length() == 0 || password.length() == 0
			|| iter_user == m_users.end())
		return;

	try {
		m_current_user = &iter_user.value();
		m_master_password.logout();
		bool logged_in = m_master_password.login(m_current_user->userData(),
				password.toUtf8().constData());
		if (!logged_in) {
			m_current_user = nullptr;
			m_ui->txtPassword->selectAll();
			m_ui->txtPassword->setFocus();
			QMessageBox::critical(this, tr("Login Failed"),
					tr("Wrong Password"));
			return;
		}
		m_ui->btnLoginLogout->setText(tr("Logout"));
		enableUI(true);
		clearSitesUI();
		for (auto& site : m_current_user->getSites()) {
			site->password_visible = m_application_settings.show_pw_after_login;
			addSiteToUI(*site);
		}
		m_proxy_model->invalidate();
		if (m_proxy_model->rowCount() > 0)
			m_ui->tblSites->selectRow(0);

		//clear the password: no need to store it anymore
		m_ui->txtPassword->setText("");
		for (int i = 0; i < password.length(); ++i)
			password[i] = '\0';
		m_ui->txtFilter->setFocus();
		if (m_application_settings.show_identicon)
			m_hide_identicon_timer->start(2000);

	} catch(CryptoException& e) {
		QString error_msg;
		switch(e.type) {
		case CryptoException::Type_HMAC_SHA256_failed:
			error_msg = tr("HMAC SHA256 function failed");
			break;
		case CryptoException::Type_scrypt_failed:
			error_msg = tr("scrypt function failed");
			break;
		case CryptoException::Type_not_logged_in:
			error_msg = tr("not logged in");
			break;
		case CryptoException::Type_thread_exception:
			error_msg = tr("Multithreadding exception");
			break;
		case CryptoException::Type_random_failed:
			error_msg = tr("Random number generation failed");
			break;
		}
		QMessageBox::critical(this, tr("Cryptographic exception"),
			tr("Error: %1.\n"
			"Should it happen again, then something is seriously wrong with the program installation.").arg(error_msg));
	}
}

void MainWindow::logout() {
	LOG(LogLevel::Debug, "Logout");
	m_master_password.logout();
	m_current_user = nullptr;
	m_ui->btnLoginLogout->setText(tr("Login"));
	m_ui->txtPassword->setText("");
	m_ui->txtFilter->setText("");
	if (m_clipboard_timer->isActive())
		clearDataFromClipboard();
	enableUI(false);
	clearSitesUI();
	m_ui->txtPassword->setFocus();
}

void MainWindow::enableUI(bool logged_in) {
	m_ui->cmbUserName->setEnabled(!logged_in);
	m_ui->txtPassword->setEnabled(!logged_in);
	m_ui->btnAddSite->setEnabled(logged_in);
}

void MainWindow::addUser() {
	UserWidget add_user(UserWidget::Type_new, this);
	if (add_user.exec() == 1) { //accepted
		QString user_name = add_user.userName();
		if (m_users.find(user_name) != m_users.end()) {
			QMessageBox::critical(this, tr("User exists"),
					tr("Error: user already exists."));
			return;
		}
		UiUser user(user_name);
		add_user.applyData(user);

		m_users.insert(user_name, user);
		m_ui->txtPassword->setText(add_user.password());
		m_ui->cmbUserName->addItem(user_name);
		m_ui->cmbUserName->setCurrentIndex(m_ui->cmbUserName->count() - 1);
		m_ui->btnDeleteUser->setEnabled(true);
		m_ui->btnEditUser->setEnabled(true);
		login();
	}
}
void MainWindow::editUser() {
	UserWidget user_widget(UserWidget::Type_edit, this);

	QString user_name = m_ui->cmbUserName->currentText();
	auto iter_user = m_users.find(user_name);
	if (iter_user == m_users.end())
		return;
	UiUser& user = iter_user.value();
	user_widget.setData(user);

	if (user_widget.exec() == 1) { //accepted
		user_widget.applyData(user);
	}
}

void MainWindow::deleteUser() {
	int selected = m_ui->cmbUserName->currentIndex();
	if (selected >= 0) {
		QString user_name = m_ui->cmbUserName->itemText(selected);
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, tr("Delete User"),
				tr("Do you really want to delete the user %1?").arg(user_name),
				QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			logout();
			m_ui->cmbUserName->removeItem(selected);
			m_users.remove(user_name);
		}
	}
	if (m_ui->cmbUserName->count() == 0) {
		m_ui->btnDeleteUser->setEnabled(false);
		m_ui->btnEditUser->setEnabled(false);
	}
}

void MainWindow::showGeneratePasswordDialog() {
	PasswordGeneratorWidget password_generator(this);
	password_generator.exec();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	if (m_tray_icon && m_tray_icon->isVisible()) {
		hide();
		activateLogoutTimer();
		event->ignore();
	} else {
		QMainWindow::closeEvent(event);
	}
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
	if (obj == m_ui->tblSites && event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		bool handled_key = false;
		auto changeSelection = [this] (int offset) {
			QModelIndex selected_row = getSelectedRow();
			if (selected_row.isValid()) {
				m_ui->tblSites->selectRow(
					(selected_row.row() + offset + m_proxy_model->rowCount())
					% m_proxy_model->rowCount());
			}
		};
		int key = keyEvent->key();
		if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt
				|| key == Qt::Key_Meta || key == Qt::Key_unknown)
			return false;

		QKeySequence key_sequence = QKeySequence(keyEvent->modifiers() | key);

		for (int i = 0; i < (int)m_table_shortcuts.size(); ++i) {
			for (auto& key_shortcut : m_table_shortcuts[i]) {
				if (key_shortcut.matches(key_sequence) == QKeySequence::ExactMatch) {
					switch ((ShortcutAction)i) {
					case ShortcutAction::CopyPWToClipboard:
					{
						TableItem* item = getSelectedItem();
						if (item)
							copyPWToClipboard(item->site());
					}
						break;
					case ShortcutAction::CopyUserToClipboard:
					{
						TableItem* item = getSelectedItem();
						if (item)
							copyUserToClipboard(item->site());
					}
						break;
					case ShortcutAction::FillForm:
						fillForm();
						break;
					case ShortcutAction::FillFormPasswordOnly:
						fillForm(true);
						break;
					case ShortcutAction::SelectFilter:
						m_ui->txtFilter->selectAll();
						m_ui->txtFilter->setFocus();
						break;
					case ShortcutAction::PreviousItem:
						changeSelection(-1);
						break;
					case ShortcutAction::NextItem:
						changeSelection(+1);
						break;
					case ShortcutAction::OpenURL:
						openSelectedUrl();
						break;
					case ShortcutAction::Logout:
						logout();
						break;
					default:
						THROW(EINVALID_PARAMETER);
					}
					handled_key = true;
				}
			}
		}
		return handled_key;
	}
	return false;
}
void MainWindow::fillForm(bool password_only) {
	TableItem* item = getSelectedItem();
	if (item) {
		string password = m_master_password.sitePassword(item->site().site);
		QClipboard* clipboard = QApplication::clipboard();
		QString original_text = clipboard->text();

		m_keypress.releaseModifiers();

		if (m_application_settings.form_fill_hide_window)
			showHide();
		else
			m_keypress.altTab();

		//wait a bit for the window manager to react
		thread()->msleep(500);

		QString quser_name = item->site().user_name;
		if (!quser_name.isEmpty() && !password_only) {
			const char* user_name = quser_name.toUtf8().constData();
			m_keypress.type(user_name);
			m_keypress.type("\t");
		}
		m_keypress.type(password.c_str());
		m_keypress.type("\n");

		//Note: we don't restore modifiers, because in the meanwhile
		//the user probably already released them
	}
}

void MainWindow::openSelectedUrl() {
	TableItem* item = getSelectedItem();
	if (!item || item->site().url == "") return;
	QDesktopServices::openUrl(QUrl(item->site().url, QUrl::TolerantMode));
}
void MainWindow::openUrlClicked() {
	UserPushButton* button = dynamic_cast<UserPushButton*>(sender());
	if (!button || button->site().url == "") return;
	QDesktopServices::openUrl(QUrl(button->site().url, QUrl::TolerantMode));
}
void MainWindow::appAboutToQuit() {
	if (m_clipboard_timer->isActive())
		clearDataFromClipboard();
	saveSettings();
}
void MainWindow::saveSettings() {
	LOG(LogLevel::Debug, "saving settings");
	QSettings settings("qMasterPassword", "qMasterPassword");
	settings.setValue("main_window/geometry", saveGeometry());
	settings.setValue("main_window/window_state", saveState());
	settings.setValue("main_window/selected_user",
			m_ui->cmbUserName->currentText());
	settings.setValue("main_window/table_header_state",
			m_ui->tblSites->horizontalHeader()->saveState());

	QByteArray categories_data;
	QDataStream categories_stream(&categories_data, QIODevice::ReadWrite);
	categories_stream.setVersion(QDataStream::Qt_4_8);
	categories_stream << m_categories;
	settings.setValue("data/categories", categories_data);

	QByteArray user_data;
	QDataStream stream(&user_data, QIODevice::ReadWrite);
	stream.setVersion(QDataStream::Qt_4_8);
	stream << m_users;
	settings.setValue("data/users", user_data);
	m_application_settings.saveSettings(settings);
}
void MainWindow::readSettings() {
	QSettings settings("qMasterPassword", "qMasterPassword");
	restoreGeometry(settings.value("main_window/geometry").toByteArray());
	restoreState(settings.value("main_window/window_state").toByteArray());
	m_ui->tblSites->horizontalHeader()->restoreState(
			settings.value("main_window/table_header_state").toByteArray());

    QDataStream categories_stream(settings.value("data/categories").toByteArray());
	categories_stream.setVersion(QDataStream::Qt_4_8);
    categories_stream >> m_categories;
	if (m_categories.isEmpty()) {
		//add default categories
		QList<QString> categories = SettingsWidget::defaultCategories();
		m_categories[m_next_category_id++] = tr("All");
		for (const auto& category : categories)
			m_categories[m_next_category_id++] = category;
	} else {
		m_next_category_id = 0;
		for (auto category : m_categories.keys()) {
			if (category > m_next_category_id)
				m_next_category_id = category;
		}
		++m_next_category_id;
		LOG(LogLevel::Debug, "next category id: %i", (int )m_next_category_id);
    }
	for (auto iter = m_categories.begin(); iter != m_categories.end(); ++iter) {
		addCategory(iter.value(), iter.key());
	}
	selectCategory(0);

    QDataStream stream(settings.value("data/users").toByteArray());
	stream.setVersion(QDataStream::Qt_4_8);
    QString selected_user = settings.value("main_window/selected_user").toString();
    stream >> m_users;
    int selected_index = -1;
	for (auto& user : m_users) {
		LOG(LogLevel::Debug, "Read user: %s (%i sites)",
			user.getUserName().toUtf8().constData(), user.getSites().count());

		m_ui->cmbUserName->addItem(user.getUserName());
		if (user.getUserName() == selected_user)
			selected_index = m_ui->cmbUserName->count() - 1;
    }
	if (selected_index != -1)
		m_ui->cmbUserName->setCurrentIndex(selected_index);
	if (!m_users.isEmpty())
		m_ui->txtPassword->setFocus();
	m_application_settings.restoreSettings(settings);
}

void MainWindow::addSite() {
	if (!m_current_user) return;
	shared_ptr<UiSite> site = make_shared<UiSite>();
	site->password_visible = m_application_settings.show_pw_after_login;
	EditSiteWidget edit_site(m_categories, *site, EditSiteWidget::Type_new, this);
	if (edit_site.exec() == 1) { //accepted
		edit_site.applyData();
		m_current_user->getSites().append(site);
		addSiteToUI(*site.get());
		saveSettings();
	}
}

void MainWindow::deleteSite() {
	if (!m_current_user) return;
	TableItem* item = getSelectedItem();
	if (!item) return;
	m_sites_model->removeRow(item->row());
	for (auto iter = m_current_user->getSites().begin();
			iter != m_current_user->getSites().end(); ++iter) {
		if(iter->get() == &item->site()) {
			m_current_user->getSites().erase(iter);
			saveSettings();
			break;
		}
	}
}

void MainWindow::editSite() {
	if (!m_current_user) return;
	TableItem* item = getSelectedItem();
	if (!item) return;
	UiSite& site = item->site();

	EditSiteWidget edit_site(m_categories, site, EditSiteWidget::Type_edit, this);
	if (edit_site.exec() == 1) { //accepted
		edit_site.applyData();
		updateModelItem(item->row(), site);
		saveSettings();
	}
}

QModelIndex MainWindow::getSelectedRow() {
	QItemSelectionModel* selection = m_ui->tblSites->selectionModel();
	QModelIndexList selected_rows = selection->selectedRows();
	if (selected_rows.count() == 0)
		return QModelIndex();
	return selected_rows.at(0);
}
TableItem* MainWindow::getSelectedItem() {
	QModelIndex selected_row = getSelectedRow();
	if (!selected_row.isValid())
		return nullptr;
	return dynamic_cast<TableItem*>(m_sites_model->itemFromIndex(
			m_proxy_model->mapToSource(selected_row)));
}

void MainWindow::selectionChanged(const QItemSelection& selected,
		const QItemSelection& deselected) {
	bool has_selection = selected.count() > 0;
	m_ui->btnEditSite->setEnabled(has_selection);
	m_ui->btnDeleteSite->setEnabled(has_selection);
}

void MainWindow::addCategory(const QString& name, CategoryId id) {
	LOG(LogLevel::Debug, "Adding Category %s", name.toStdString().c_str());
	if (id == -1)
		id = m_next_category_id++;
	m_categories[id] = name;
	QPushButton* button = new CategoryButton(id, name);
	button->setCheckable(true);
    connect(button, SIGNAL(clicked()), this, SLOT(categoryButtonPressed()));
	m_ui->layoutCategories->addWidget(button);
}
void MainWindow::removeCategory(CategoryId category_id) {
	const QString& category_name = m_categories[category_id];
	LOG(LogLevel::Debug, "Removing Category %s", category_name.toStdString().c_str());
	m_categories.remove(category_id);

	//button
	for (int i = 0; i < m_ui->layoutCategories->count(); ++i) {
		CategoryButton* button = dynamic_cast<CategoryButton*>(
				m_ui->layoutCategories->itemAt(i)->widget());
		if (button && button->category_id == category_id) {
			m_ui->layoutCategories->removeWidget(button);
			button->deleteLater();
			break;
		}
	}

	//in all sites
	for (auto& user : m_users) {
		for (auto& site : user.getSites()) {
			QList<CategoryId>& category_ids = site.get()->category_ids;
			for (int i = 0; i < category_ids.count(); ++i) {
				if (category_ids[i] == category_id) {
					category_ids.removeAt(i);
					break;
				}
			}
		}
	}
}

void MainWindow::selectCategory(CategoryId category) {
	for (int i = 0; i < m_ui->layoutCategories->count(); ++i) {
		CategoryButton* button = dynamic_cast<CategoryButton*>(
				m_ui->layoutCategories->itemAt(i)->widget());
		if (button) {
			button->setChecked(button->category_id == category);
		}
	}
	m_selected_category = category;
	m_proxy_model->invalidate();
	uiSitesTableChanged();
}

void MainWindow::categoryButtonPressed() {
	CategoryButton* button = dynamic_cast<CategoryButton*>(sender());
	if (!button) return;
	selectCategory(button->category_id);
}

void MainWindow::filterTextChanged(QString filter_text) {
	m_proxy_model->setFilterRegularExpression(
			QRegularExpression(QRegularExpression::escape(filter_text), QRegularExpression::CaseInsensitiveOption));
	m_proxy_model->setFilterKeyColumn(0);
	uiSitesTableChanged();
}

void MainWindow::copyPWToClipboardClicked() {
	UserPushButton* button = dynamic_cast<UserPushButton*>(sender());
	if (!button) return;
	copyPWToClipboard(button->site());
}
void MainWindow::copyPWToClipboard(const UiSite& site) {
	LOG(LogLevel::Debug, "copy pw to clipboard");
	string password = m_master_password.sitePassword(site.site);
	int timeout = copyToClipboard(QString::fromUtf8(password.c_str()));
	QString suffix = "";
	if (timeout > 0)
		suffix = tr(" for %1 seconds").arg(timeout);
	statusBar()->showMessage(tr("Copied Password to Clipboard")+suffix, 1500);
}

void MainWindow::copyUserToClipboard(const UiSite& site) {
	if (site.user_name.isEmpty())
		return;
	LOG(LogLevel::Debug, "copy login name to clipboard");
	int timeout = copyToClipboard(site.user_name);
	QString suffix = "";
	if (timeout > 0)
		suffix = tr(" for %1 seconds").arg(timeout);
	statusBar()->showMessage(tr("Copied login name to Clipboard")+suffix, 1500);
}

int MainWindow::copyToClipboard(const QString& str) {
	QClipboard* clipboard = QApplication::clipboard();
	QString original_text = clipboard->text();
	clipboard->setText(str);
	QString suffix = "";
	if (m_application_settings.clipboard_pw_timeout > 0) {
		if (!m_clipboard_timer->isActive())
			m_clipboard_previous_data = original_text;
		m_clipboard_data = str;
		int timeout = m_application_settings.clipboard_pw_timeout;
		m_clipboard_timer->start(1000);
		m_clipboard_time_left = timeout;
		m_status_progress_bar->setMaximum(timeout);
		m_status_progress_bar->setValue(timeout);
		m_status_progress_bar->show();
		return timeout;
	}
	return 0;
}

void MainWindow::clearDataFromClipboardTimer() {
	if (--m_clipboard_time_left <= 0) {
		clearDataFromClipboard();
	} else {
		m_status_progress_bar->setValue(m_clipboard_time_left);
	}
}
void MainWindow::clearDataFromClipboard() {
	LOG(LogLevel::Debug, "Clear data from clipboard");
	QClipboard* clipboard = QApplication::clipboard();
	QString original_text = clipboard->text();
	if (original_text == m_clipboard_data) {
		clipboard->setText(m_clipboard_previous_data);
	}
	m_status_progress_bar->hide();
	m_clipboard_timer->stop();
	m_clipboard_previous_data = "";
	m_clipboard_data = "";
	m_clipboard_time_left = 0;
}
void MainWindow::showHidePWClicked() {
	LOG(LogLevel::Debug, "show/hide PW clicked");
	UserPushButton* button = dynamic_cast<UserPushButton*>(sender());
	if (!button) return;
	UiSite& site = button->site();
	site.password_visible = !site.password_visible;
	button->setIcon(QIcon(site.password_visible ? ":/hidden.png" : ":/shown.png"));

	for (int i = 0; i < m_sites_model->rowCount(); ++i) {
		TableItem* item = dynamic_cast<TableItem*>(m_sites_model->item(i, 0));
		if (item && &item->site() == &site) {
			updateModelItem(i, site);
			break;
		}
	}
}

void MainWindow::uiSitesTableChanged() {
	//(re-)open persistent editors, because after refilter, they get hidden.
	//yes it's ugly, but i didn't see an easier way...
	for (int row = 0; row < m_proxy_model->rowCount(); ++row) {
		m_ui->tblSites->openPersistentEditor(
			m_proxy_model->index(row, m_copy_column_idx));
	}
	m_ui->tblSites->resizeColumnToContents(m_copy_column_idx);
	if (m_proxy_model->rowCount() > 0
			&& m_ui->tblSites->selectionModel()->selectedRows().count() == 0)
		m_ui->tblSites->selectRow(0);
}
void MainWindow::activateLogoutTimer() {
	if (!m_master_password.isLoggedIn() ||
		!m_application_settings.auto_logout_when_hidden) return;

	LOG(LogLevel::Debug, "Activating Logout Timer (%i min)",
			m_application_settings.auto_logout_timeout);
	m_logout_timer->start(m_application_settings.auto_logout_timeout*60*1000);
}

void MainWindow::abortLogoutTimer() {
	if (m_logout_timer->isActive()) {
		LOG(LogLevel::Debug, "Aborting Logout Timer");
		m_logout_timer->stop();
	}
}

void MainWindow::showHide() {
	setVisible(!isVisible());
	if (isVisible()) {
		abortLogoutTimer();
		if (m_master_password.isLoggedIn())
			m_ui->tblSites->setFocus();
		else
			m_ui->txtPassword->setFocus();
	} else activateLogoutTimer();
}

void MainWindow::iconAboutToShow() {
	if (m_tray_icon_show_action) {
		if (isVisible()) {
			m_tray_icon_show_action->setText(tr("&Hide"));
		} else {
			m_tray_icon_show_action->setText(tr("&Show"));
		}
	}
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		showHide();
		break;
	default:
		break;
	}
}
void MainWindow::showSettingsWidget() {
	SettingsWidget settings_widget(m_application_settings, m_users, m_categories,
			m_import_export, this);
	connect(&settings_widget, SIGNAL(showTrayIconChanged(bool)),
			this, SLOT(showTrayIcon(bool)));
	//on import we would need to update the ui. but simply logout is easier :)
	connect(&settings_widget, SIGNAL(dataImported(QString)), this, SLOT(logout()));

	if (settings_widget.exec() == 1) {
		QList<QString> categories = settings_widget.categories();
		for (const auto& category_id : m_categories.keys()) {
			if (category_id == 0) continue;
			const QString& current_category = m_categories[category_id];
			bool found = false;
			for (int category = 0; category < categories.count(); ++category) {
				if (categories[category] == current_category) {
					found = true;
					categories.removeAt(category);
					break;
				}
			}
			if (!found)
				removeCategory(category_id);
		}
		for (const auto& category_name : categories) {
			addCategory(category_name);
		}
	}
}
void MainWindow::showAboutWidget() {

	static const char* about_msg =
	"<p><b>%1 version %2 </b></p>"
	"<p>%5</p>"
	"<p>Use and redistribute under the terms of the<br>%6</p>"
	"<p>Source code: %7</p>"
	"<p>This version was compiled against Qt %3 and runs under Qt %4</p>"
	"<p>Part of the keypress code was taken from xdotool (linux):<br/>"
	"Copyright (c) 2007, 2008, 2009: Jordan Sissel <br/>"
	"and Windows Input Simulator (Windows):<br/> Copyright(c) 1998-2012, Arnaud Colin</p>";

	QMessageBox::about(this, tr("About %1").arg(APP_NAME),
		tr(about_msg).arg(APP_NAME).arg(QString::fromStdString(getAppVersion().toStr()))
		.arg(QT_VERSION_STR).arg(qVersion())
		.arg("Copyright (c) 2015 Beat Küng <a href=\"mailto:beat-kueng@gmx.net\">beat-kueng@gmx.net</a>")
		.arg("<a href=\"http://www.gnu.org/licenses/gpl.html\">GNU General Public License Version 3</a>")
		.arg("<a href=\"https://github.com/bkueng/qMasterPassword\">github.com/bkueng/qMasterPassword</a>"));
}
void MainWindow::showShortcutsWidget() {
	ShortcutsWidget shortcuts_widget(*this);
	shortcuts_widget.exec();
}

void MainWindow::showTrayIcon(bool visible) {
	if (visible) {
		initTrayIcon();
		m_tray_icon->show();
	} else {
		if (m_tray_icon) {
			m_tray_icon->hide();
			delete m_tray_icon;
			m_tray_icon = nullptr;
		}
	}
}

void MainWindow::uiLoginChanged() {
	if (!m_application_settings.show_identicon || m_ui->txtPassword->text().isEmpty()) {
		m_ui->lblIdenticon->clear();
	} else {
		m_delayed_identicon_update_timer->start(300);
	}
}

void MainWindow::delayedIdenticonUpdate() {
	QColor identicon_color;
	QString identicon;
	const QString& password = m_ui->txtPassword->text();
	if (!m_application_settings.show_identicon || password.isEmpty()) {
		m_ui->lblIdenticon->clear();
	} else {
		m_identicon.setUserName(m_ui->cmbUserName->currentText());
		m_identicon.getIdenticon(password, identicon, identicon_color);
		m_ui->lblIdenticon->setText(identicon);

		QPalette palette = m_ui->lblIdenticon->palette();
		palette.setColor(QPalette::WindowText, identicon_color);
		m_ui->lblIdenticon->setPalette(palette);
	}
}

bool MySortFilterProxyModel::filterAcceptsRow(int source_row,
		const QModelIndex& source_parent) const {
	if (m_main_window.selectedCategory() != 0) {
		QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
		TableItem* item = dynamic_cast<TableItem*>(m_main_window.model()->itemFromIndex(index));
		if (item) {
			if (!item->site().category_ids.contains(m_main_window.selectedCategory()))
				return false;
		}
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

QString MainWindow::description(ShortcutAction action) {
	switch (action) {
	case ShortcutAction::CopyPWToClipboard:
		return tr("Copy password of selected item to clipboard");
	case ShortcutAction::CopyUserToClipboard:
		return tr("Copy login/user name of selected item to clipboard");
	case ShortcutAction::FillForm:
		return tr("Auto fill form: select next window and fill in user name (if set) and password");
	case ShortcutAction::FillFormPasswordOnly:
		return tr("Auto fill form (password only)");
	case ShortcutAction::SelectFilter:
		return tr("Focus the filter text");
	case ShortcutAction::PreviousItem:
		return tr("Select previous item");
	case ShortcutAction::NextItem:
		return tr("Select next item");
	case ShortcutAction::OpenURL:
		return tr("Open URL of selected item");
	case ShortcutAction::Logout:
		return tr("Logout");
	default:
		THROW(EINVALID_PARAMETER);
	}
}
