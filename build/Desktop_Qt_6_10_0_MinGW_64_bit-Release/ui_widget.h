/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QQuickWidget *quickWidget;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(1200, 640);
        Widget->setMinimumSize(QSize(1200, 640));
        Widget->setMaximumSize(QSize(1200, 640));
        quickWidget = new QQuickWidget(Widget);
        quickWidget->setObjectName("quickWidget");
        quickWidget->setGeometry(QRect(0, 0, 1200, 640));
        quickWidget->setMinimumSize(QSize(1200, 640));
        quickWidget->setMaximumSize(QSize(1200, 640));
        quickWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
