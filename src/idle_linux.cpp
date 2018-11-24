#include "idle.h"

#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <QtCore>

namespace {

const char *fdDisplayService = "org.freedesktop.ScreenSaver";
const char *fdDisplayPath = "/org/freedesktop/ScreenSaver";
const char *fdDisplayInterface = fdDisplayService;

const char *gnomeSystemService = "org.gnome.SessionManager";
const char *gnomeSystemPath = "/org/gnome/SessionManager";
const char *gnomeSystemInterface = gnomeSystemService;

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
    QDBusReply<quint32> reply = dbus.call("Inhibit", QCoreApplication::applicationName(), reason);
    return handleReply(reply);
}

bool Idle::allowDisplaySleep() {
    QDBusInterface dbus(fdDisplayService, fdDisplayPath, fdDisplayInterface);
    dbus.call("UnInhibit", cookie);
    return true;
}

QString Idle::displayErrorMessage() {
    return errorMessage;
}

bool Idle::preventSystemSleep(const QString &reason) {
    QDBusInterface dbus(gnomeSystemService, gnomeSystemPath, gnomeSystemInterface);
    QDBusReply<quint32> reply = dbus.call("Inhibit", QCoreApplication::applicationName(), reason);
    return handleReply(reply);
}

bool Idle::allowSystemSleep() {
    QDBusInterface dbus(gnomeSystemService, gnomeSystemPath, gnomeSystemInterface);
    dbus.call("UnInhibit", cookie);
    return true;
}

QString Idle::systemErrorMessage() {
    return errorMessage;
}
