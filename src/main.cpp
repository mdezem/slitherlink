/**
 * @author Michele Tomaiuolo - http://www.ce.unipr.it/people/tomamic
 * @license This software is free - http://www.gnu.org/licenses/gpl.html
 */

#include <QtWidgets>
#include <cstdlib>
#include <ctime>
#include "mainwindow.h"
#include "slitherlink.h"
#include "gamegui.h"

using namespace std;

int main(int argc, char* argv[])
{
    QApplication a{argc, argv};
    MainWindow mw;
    mw.showNormal();
    return a.exec();
}
