#include <QApplication>
#include "MainWindow.h"

/**
 * @brief Einstiegspunkt der Anwendung.
 *
 * Erzeugt die Qt-Anwendung, oeffnet das Hauptfenster (GUI-Modul) und startet
 * die Ereignisschleife. Saemtliche Fachlogik ist in den einzelnen Modulen
 * gekapselt; main() haelt lediglich den Programmablauf zusammen.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Bode-Diagramm-Viewer"));
    app.setOrganizationName(QStringLiteral("IU - DLBROEPRS01"));

    MainWindow window;
    window.show();

    return app.exec();
}
