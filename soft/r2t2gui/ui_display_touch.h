/********************************************************************************
** Form generated from reading UI file 'display_touch.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAY_TOUCH_H
#define UI_DISPLAY_TOUCH_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Display_touch
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QGraphicsView *graphicsView;

    void setupUi(QMainWindow *Display_touch)
    {
        if (Display_touch->objectName().isEmpty())
            Display_touch->setObjectName(QStringLiteral("Display_touch"));
        Display_touch->resize(800, 480);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Display_touch->sizePolicy().hasHeightForWidth());
        Display_touch->setSizePolicy(sizePolicy);
        Display_touch->setMinimumSize(QSize(640, 400));
        centralWidget = new QWidget(Display_touch);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        sizePolicy.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy);

        gridLayout->addWidget(graphicsView, 0, 0, 1, 1);

        Display_touch->setCentralWidget(centralWidget);

        retranslateUi(Display_touch);

        QMetaObject::connectSlotsByName(Display_touch);
    } // setupUi

    void retranslateUi(QMainWindow *Display_touch)
    {
        Display_touch->setWindowTitle(QApplication::translate("Display_touch", "R2T2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Display_touch: public Ui_Display_touch {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAY_TOUCH_H
