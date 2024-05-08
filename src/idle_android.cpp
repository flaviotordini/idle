#include "idle.h"

#include <QtCore>

namespace {
const int FLAG_KEEP_SCREEN_ON = 128;

QJniObject getWindow() {
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (activity.isValid()) {
        QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        return window;
    }
    return QJniObject();
}

QJniObject getPowerManager() {
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject powerString = QJniObject::fromString("power");
    QJniObject powerManager =
            activity.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",
                                      powerString.object<jstring>());
    return powerManager;
}

QJniObject wakeLock;

} // namespace

bool Idle::preventDisplaySleep(const QString &reason) {
    Q_UNUSED(reason)
    QJniObject window = getWindow();
    if (!window.isValid()) return false;
    window.callMethod<void>("addFlags", FLAG_KEEP_SCREEN_ON);
    return true;
}

bool Idle::allowDisplaySleep() {
    QJniObject window = getWindow();
    if (!window.isValid()) return false;
    window.callMethod<void>("clearFlags", FLAG_KEEP_SCREEN_ON);
    return true;
}

QString Idle::displayErrorMessage() {
    return QString();
}

bool Idle::preventSystemSleep(const QString &reason) {
    Q_UNUSED(reason)
    if (wakeLock.isValid()) return true;
    QJniObject powerManager = getPowerManager();
    if (!powerManager.isValid()) return false;
    jint levelAndFlags =
            QJniObject::getStaticField<jint>("android/os/PowerManager", "PARTIAL_WAKE_LOCK");
    QJniObject tag = QJniObject::fromString(QCoreApplication::applicationName());
    wakeLock = powerManager.callObjectMethod(
            "newWakeLock", "(ILjava/lang/String;)Landroid/os/PowerManager$WakeLock;", levelAndFlags,
            tag.object<jstring>());
    wakeLock.callMethod<void>("acquire");
    return true;
}

bool Idle::allowSystemSleep() {
    if (!wakeLock.isValid()) return false;
    wakeLock.callMethod<void>("release");
    wakeLock = QJniObject();
    return true;
}

QString Idle::systemErrorMessage() {
    return QString();
}
