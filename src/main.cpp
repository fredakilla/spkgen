#include "MainWindow.h"
#include <QApplication>
#include <gplay-engine.h>

#include <QFile>
#include <QStyleFactory>

//#include "path.hpp"
#include "GraphView.h"


// TODO
/*
- [x] implement base nodes classes
- [x] show 3D debug zones geometries
- [x] serialize node widget's parameters
- [ ] add +,- buttons to add/remove dynamic inputs on some nodes using lists (emitters, groups)
- [ ] graph plot editor widget for spark graph interpolators
- [ ] complete all spark nodes
- [ ] interact with 3D view (camera move...)
- [ ] multiple scenes managment
- [ ] possibility to attach result effect to scene nodes (static node, node in motion, trow bullet, ...)
- [ ] add pliable parameters button to show/hide parameters widgets
- [ ] comments items on node view
- [ ] set styles
*/

#include <thirdparty/splines/Splines.hh>
#include "benchmark.h"
#include <iostream>

using namespace SplinesLoad;



float randomFloat()
{
    const float LO = -500.0f;
    const float HI = 500.0f;
    return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}

void benchmarkSplines()
{
    srand (static_cast <unsigned> (time(0)));

    const int KEYCOUNT = 100;
    const int LOOP = 1000000;

    double* data1 = new double[KEYCOUNT];
    double* data2 = new double[KEYCOUNT];

    GP_BENCHMARK_BLOCK_BEGIN("generate random data");
    for(int i=0; i<KEYCOUNT; i++) {
        data1[i] = i;
        data2[i] = randomFloat();
    }
    GP_BENCHMARK_BLOCK_END();

    LinearSpline linearSpline;
    ConstantSpline constantSpline;
    CubicSpline cubicSpline;
    AkimaSpline akimaSpline;
    BesselSpline besselSpline;
    PchipSpline pchipSpline;
    QuinticSpline quinticSpline;

    GP_BENCHMARK_FUNC(linearSpline.build(data1, data2, KEYCOUNT),   1);
    GP_BENCHMARK_FUNC(constantSpline.build(data1, data2, KEYCOUNT), 1);
    GP_BENCHMARK_FUNC(cubicSpline.build(data1, data2, KEYCOUNT),    1);
    GP_BENCHMARK_FUNC(akimaSpline.build(data1, data2, KEYCOUNT),    1);
    GP_BENCHMARK_FUNC(besselSpline.build(data1, data2, KEYCOUNT),   1);
    GP_BENCHMARK_FUNC(pchipSpline.build(data1, data2, KEYCOUNT),    1);
    GP_BENCHMARK_FUNC(quinticSpline.build(data1, data2, KEYCOUNT),  1);

    //float key = randomFloat();
    float key = KEYCOUNT-1;
    printf("evaluate key %f\n", key);

    GP_BENCHMARK_FUNC(linearSpline.eval(key), LOOP);
    GP_BENCHMARK_FUNC(constantSpline.eval(key), LOOP);
    GP_BENCHMARK_FUNC(cubicSpline.eval(key), LOOP);
    GP_BENCHMARK_FUNC(akimaSpline.eval(key), LOOP);
    GP_BENCHMARK_FUNC(besselSpline.eval(key), LOOP);
    GP_BENCHMARK_FUNC(pchipSpline.eval(key), LOOP);
    GP_BENCHMARK_FUNC(quinticSpline.eval(key), LOOP);



    //--------------------

   /* ePath pathLinear;
    ePath pathCubic;
    ePath pathStep;
    ePath mixedStep;

    for(int i=0; i<KEYCOUNT; i++)
    {
        pathLinear.addKey(i, randomFloat(), ePathKeyInterpol::ePI_LINEAR);
        pathCubic.addKey(i, randomFloat(), ePathKeyInterpol::ePI_CUBIC);
        pathStep.addKey(i, randomFloat(), ePathKeyInterpol::ePI_STEP);
        mixedStep.addKey(i, randomFloat(), ePathKeyInterpol(rand()%3));
    }

    GP_BENCHMARK_FUNC(pathLinear.evaluate(key), LOOP);
    GP_BENCHMARK_FUNC(pathCubic.evaluate(key),  LOOP);
    GP_BENCHMARK_FUNC(pathStep.evaluate(key),   LOOP);
    GP_BENCHMARK_FUNC(mixedStep.evaluate(key),  LOOP);



    //-------------
    // path4sampler
    //-------------


    ePath4 path4;
    for(int i=0; i<KEYCOUNT; i++)
    {
        path4.getSubPath(0).addKey(i, randomFloat(), ePathKeyInterpol::ePI_LINEAR);
        path4.getSubPath(1).addKey(i, randomFloat(), ePathKeyInterpol::ePI_CUBIC);
        path4.getSubPath(2).addKey(i, randomFloat(), ePathKeyInterpol::ePI_STEP);
        path4.getSubPath(3).addKey(i, randomFloat(), ePathKeyInterpol(rand()%3));
    }
    ePath4Sampler path4Sampler;
    path4Sampler.sample(path4);

    GP_BENCHMARK_FUNC(path4Sampler.evaluate(key), LOOP);
*/


    GP_BENCHMARK_REPORT();
}

int main(int argc, char **argv)
{
    //benchmarkSplines(); return 0;




    gplay::Platform::setArguments(&argc, &argv);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("fredakilla");
    QCoreApplication::setApplicationName("spkgen");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);


    QFile cssFile("estudio4.css");
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);

    if (cssFile.open(QFile::ReadOnly))
        qApp->setStyleSheet(QString(cssFile.readAll()));


    MainWindow mainWindow;
    mainWindow.showMaximized();


    /*GraphView graphEditor;
    graphEditor.setGeometry(0,0,500,300);
    graphEditor.show();*/


    int ret = app.exec();

    return ret;
}
