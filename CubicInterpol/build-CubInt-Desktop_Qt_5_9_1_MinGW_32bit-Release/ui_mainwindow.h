/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <drawarea.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    DrawArea *drawArea;
    QGridLayout *vertLayout;
    QRadioButton *radioPolynom;
    QPushButton *clearButton;
    QLabel *ymaxLabel;
    QPushButton *drawButton;
    QLabel *dummyLabel;
    QLineEdit *xmin;
    QLabel *yminLabel;
    QLineEdit *ymin;
    QLineEdit *xmax;
    QLabel *ymax;
    QLabel *xmaxLabel;
    QLabel *xminLabel;
    QPushButton *closeButton;
    QRadioButton *radioC1Spline;
    QRadioButton *radioC2Spline;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(928, 564);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        drawArea = new DrawArea(centralWidget);
        drawArea->setObjectName(QStringLiteral("drawArea"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(drawArea->sizePolicy().hasHeightForWidth());
        drawArea->setSizePolicy(sizePolicy);
        drawArea->setMinimumSize(QSize(640, 480));
        drawArea->setBaseSize(QSize(800, 600));

        gridLayout->addWidget(drawArea, 0, 0, 1, 1);

        vertLayout = new QGridLayout();
        vertLayout->setSpacing(6);
        vertLayout->setObjectName(QStringLiteral("vertLayout"));
        vertLayout->setSizeConstraint(QLayout::SetMaximumSize);
        radioPolynom = new QRadioButton(centralWidget);
        radioPolynom->setObjectName(QStringLiteral("radioPolynom"));

        vertLayout->addWidget(radioPolynom, 4, 0, 1, 1);

        clearButton = new QPushButton(centralWidget);
        clearButton->setObjectName(QStringLiteral("clearButton"));

        vertLayout->addWidget(clearButton, 2, 1, 1, 1);

        ymaxLabel = new QLabel(centralWidget);
        ymaxLabel->setObjectName(QStringLiteral("ymaxLabel"));
        ymaxLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vertLayout->addWidget(ymaxLabel, 1, 2, 1, 1);

        drawButton = new QPushButton(centralWidget);
        drawButton->setObjectName(QStringLiteral("drawButton"));

        vertLayout->addWidget(drawButton, 2, 3, 1, 1);

        dummyLabel = new QLabel(centralWidget);
        dummyLabel->setObjectName(QStringLiteral("dummyLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dummyLabel->sizePolicy().hasHeightForWidth());
        dummyLabel->setSizePolicy(sizePolicy1);

        vertLayout->addWidget(dummyLabel, 5, 0, 1, 1);

        xmin = new QLineEdit(centralWidget);
        xmin->setObjectName(QStringLiteral("xmin"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(xmin->sizePolicy().hasHeightForWidth());
        xmin->setSizePolicy(sizePolicy2);
        xmin->setMinimumSize(QSize(80, 0));
        xmin->setMaximumSize(QSize(100, 16777215));

        vertLayout->addWidget(xmin, 0, 1, 1, 1);

        yminLabel = new QLabel(centralWidget);
        yminLabel->setObjectName(QStringLiteral("yminLabel"));
        yminLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vertLayout->addWidget(yminLabel, 1, 0, 1, 1);

        ymin = new QLineEdit(centralWidget);
        ymin->setObjectName(QStringLiteral("ymin"));
        sizePolicy2.setHeightForWidth(ymin->sizePolicy().hasHeightForWidth());
        ymin->setSizePolicy(sizePolicy2);
        ymin->setMinimumSize(QSize(80, 0));
        ymin->setMaximumSize(QSize(100, 16777215));

        vertLayout->addWidget(ymin, 1, 1, 1, 1);

        xmax = new QLineEdit(centralWidget);
        xmax->setObjectName(QStringLiteral("xmax"));
        sizePolicy2.setHeightForWidth(xmax->sizePolicy().hasHeightForWidth());
        xmax->setSizePolicy(sizePolicy2);
        xmax->setMinimumSize(QSize(80, 0));
        xmax->setMaximumSize(QSize(100, 16777215));

        vertLayout->addWidget(xmax, 0, 3, 1, 1);

        ymax = new QLabel(centralWidget);
        ymax->setObjectName(QStringLiteral("ymax"));
        ymax->setMinimumSize(QSize(80, 0));
        ymax->setMaximumSize(QSize(100, 16777215));
        ymax->setFrameShape(QFrame::Panel);

        vertLayout->addWidget(ymax, 1, 3, 1, 1);

        xmaxLabel = new QLabel(centralWidget);
        xmaxLabel->setObjectName(QStringLiteral("xmaxLabel"));
        xmaxLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vertLayout->addWidget(xmaxLabel, 0, 2, 1, 1);

        xminLabel = new QLabel(centralWidget);
        xminLabel->setObjectName(QStringLiteral("xminLabel"));
        xminLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vertLayout->addWidget(xminLabel, 0, 0, 1, 1);

        closeButton = new QPushButton(centralWidget);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        vertLayout->addWidget(closeButton, 4, 3, 1, 1);

        radioC1Spline = new QRadioButton(centralWidget);
        radioC1Spline->setObjectName(QStringLiteral("radioC1Spline"));

        vertLayout->addWidget(radioC1Spline, 4, 1, 1, 1);

        radioC2Spline = new QRadioButton(centralWidget);
        radioC2Spline->setObjectName(QStringLiteral("radioC2Spline"));

        vertLayout->addWidget(radioC2Spline, 4, 2, 1, 1);


        gridLayout->addLayout(vertLayout, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 928, 27));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Plot Function", Q_NULLPTR));
        radioPolynom->setText(QApplication::translate("MainWindow", "Polynom", Q_NULLPTR));
        clearButton->setText(QApplication::translate("MainWindow", "Clear", Q_NULLPTR));
        ymaxLabel->setText(QApplication::translate("MainWindow", "ymax:", Q_NULLPTR));
        drawButton->setText(QApplication::translate("MainWindow", "Draw", Q_NULLPTR));
        dummyLabel->setText(QString());
        yminLabel->setText(QApplication::translate("MainWindow", "ymin:", Q_NULLPTR));
        ymax->setText(QString());
        xmaxLabel->setText(QApplication::translate("MainWindow", "xmax:", Q_NULLPTR));
        xminLabel->setText(QApplication::translate("MainWindow", "xmin:", Q_NULLPTR));
        closeButton->setText(QApplication::translate("MainWindow", "Close", Q_NULLPTR));
        radioC1Spline->setText(QApplication::translate("MainWindow", "C1 Spline", Q_NULLPTR));
        radioC2Spline->setText(QApplication::translate("MainWindow", "C2 Spline", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
