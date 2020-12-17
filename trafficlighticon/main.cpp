#include <QApplication>
#include <QtGui/QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <stdio.h>

enum class Status {
    Neutral,
    Green,
    Yellow,
    Red,
};

// Source: https://riptutorial.com/qt/example/21783/using-qtimer-to-run-code-on-main-thread
void DispatchToMainThread(std::function<void()> callback)
{
    // any thread
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=]()
    {
        // main thread
        callback();
        timer->deleteLater();
    });
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPixmap oPixmap(32,32);
    //QMenu* menu1 = new QMenu(); // want to get a context menu from system tray
    oPixmap.load (":/icons/icons/neutral.png");

    QIcon oIcon( oPixmap );

    QMenu* cm = new QMenu("Context menu");
    QAction* ac = cm->addAction("Quit");
    QObject::connect(ac, &QAction::triggered, &app, &QApplication::exit);

    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(oIcon);
    trayIcon->setContextMenu( cm);
    trayIcon->setVisible(true);

    QThread* t = QThread::create([trayIcon, &app] {
        QFile in;
        in.open(stdin, QIODevice::ReadOnly);
        while(in.isReadable() && !in.atEnd()) {
            QString line = in.readLine();
            line = line.trimmed().toLower();
            qDebug() << line;

            Status status = Status::Neutral;

            if      (line.startsWith("red")) { status = Status::Red; }
            else if (line.startsWith("r")) { status = Status::Red; }
            else if (line.startsWith("0")) { status = Status::Red; }
            else if (line.startsWith("error")) { status = Status::Red; }
            else if (line.startsWith("fail")) { status = Status::Red; }
            else if (line.startsWith("failure")) { status = Status::Red; }
            else if (line.startsWith("unavailable")) { status = Status::Red; }
            else if (line.startsWith("unreachable")) { status = Status::Red; }
            else if (line.startsWith("broken")) { status = Status::Red; }
            else if (line.startsWith("stopped")) { status = Status::Red; }
            else if (line.startsWith("exited")) { status = Status::Red; }
            else if (line.startsWith("-")) { status = Status::Red; }
            else if (line.startsWith("❤️")) { status = Status::Red; }
            else if (line.startsWith("🟥")) { status = Status::Red; }
            else if (line.startsWith("🔴")) { status = Status::Red; }
            else if (line.startsWith("🛑")) { status = Status::Red; }
            else if (line.startsWith("⛔")) { status = Status::Red; }

            else if (line.startsWith("green")) { status = Status::Green; }
            else if (line.startsWith("+")) { status = Status::Green; }
            else if (line.startsWith("1")) { status = Status::Green; }
            else if (line.startsWith("ok")) { status = Status::Green; }
            else if (line.startsWith("success")) { status = Status::Green; }
            else if (line.startsWith("available")) { status = Status::Green; }
            else if (line.startsWith("reachable")) { status = Status::Green; }
            else if (line.startsWith("working")) { status = Status::Green; }
            else if (line.startsWith("running")) { status = Status::Green; }
            else if (line.startsWith("🟩")) { status = Status::Green; }
            else if (line.startsWith("🟢")) { status = Status::Green; }
            else if (line.startsWith("💚")) { status = Status::Green; }

            else if (line.startsWith("yellow")) { status = Status::Yellow; }
            else if (line.startsWith("lagging")) { status = Status::Yellow; }
            else if (line.startsWith("iffy")) { status = Status::Yellow; }
            else if (line.startsWith("problems")) { status = Status::Yellow; }
            else if (line.startsWith("warning")) { status = Status::Yellow; }
            else if (line.startsWith("2")) { status = Status::Yellow; }
            else if (line.startsWith("problem")) { status = Status::Yellow; }
            else if (line.startsWith("exiting")) { status = Status::Yellow; }
            else if (line.startsWith("starting")) { status = Status::Yellow; }
            else if (line.startsWith("booting")) { status = Status::Yellow; }
            else if (line.startsWith("🚧")) { status = Status::Yellow; }
            else if (line.startsWith("🟡")) { status = Status::Yellow; }
            else if (line.startsWith("🟨")) { status = Status::Yellow; }
            else if (line.startsWith("💛")) { status = Status::Yellow; }

            QString resource;

            switch (status) {
                case Status::Red:     resource=":/icons/icons/red.png"; break;
                case Status::Yellow:  resource=":/icons/icons/yellow.png"; break;
                case Status::Green:   resource=":/icons/icons/green.png"; break;
                case Status::Neutral: resource=":/icons/icons/neutral.png"; break;
            }

            DispatchToMainThread([trayIcon, resource] {
                QPixmap oPixmap(32,32);
                oPixmap.load (resource);
                QIcon oIcon( oPixmap );
                trayIcon->setIcon(oIcon);
            });
        }
        app.quit();
    });
    t->start();
    //trayIcon->showMessage("Test Message", "Text", QSystemTrayIcon::Information, 1000);


    return app.exec();
}
