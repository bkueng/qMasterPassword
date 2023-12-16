#! /usr/bin/env python

# show/hide the main window of a running instance.
# useful to be registered as a hotkey

import dbus

bus = dbus.SessionBus()
service = bus.get_object('org.bkueng.qMasterPassword', '/MainWindow')
show_hide_func = service.get_dbus_method('showHide')
show_hide_func()
