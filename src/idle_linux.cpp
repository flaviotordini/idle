#include "idle.h"

#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <QtCore>

namespace {

const QString fdDisplayService = "org.freedesktop.ScreenSaver";
const QString fdDisplayPath = "/org/freedesktop/ScreenSaver";
const QString fdDisplayInterface = fdDisplayService;

const QString gnomeSystemService = "org.gnome.SessionManager";
const QString gnomeSystemPath = "/org/gnome/SessionManager";
const QString gnomeSystemInterface = gnomeSystemService;

const QString inhibitMethod = "Inhibit";
const QString uninhibitMethod = "UnInhibit";

quint32 cookie;
QString errorMessage;

bool handleReply(const QDBusReply<quint32> &reply) {
    if (reply.isValid()) {
        cookie = reply.value();
        errorMessage.clear();
        return true;
    }
    errorMessage = reply.error().message();
    return false;
}

} // namespace

bool Idle::preventDisplaySleep(const QString &reason) {
    QDBusInterface dbus(fdDisplayService, fdDisplayPath, fdDisplayInterface);
    QDBusReply<quint32> reply =
            dbus.call(inhibitMethod, QCoreApplication::applicationName(), reason);
    return handleReply(reply);
}

bool Idle::allowDisplaySleep() {
    QDBusInterface dbus(fdDisplayService, fdDisplayPath, fdDisplayInterface);
    dbus.call(uninhibitMethod, cookie);
    return true;
}

QString Idle::displayErrorMessage() {
    return errorMessage;
}

bool Idle::preventSystemSleep(const QString &reason) {
    QDBusInterface dbus(gnomeSystemService, gnomeSystemPath, gnomeSystemInterface);
    QDBusReply<quint32> reply =
            dbus.call(inhibitMethod, QCoreApplication::applicationName(), reason);
    return handleReply(reply);
}

bool Idle::allowSystemSleep() {
    QDBusInterface dbus(gnomeSystemService, gnomeSystemPath, gnomeSystemInterface);
    dbus.call(uninhibitMethod, cookie);
    return true;
}

QString Idle::systemErrorMessage() {
    return errorMessage;
}
