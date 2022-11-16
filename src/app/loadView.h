/*
    SPDX-FileCopyrightText: 2009 David Edmundson <kde@davidedmundson.co.uk>
    SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef LOADVIEW_H
#define LOADVIEW_H

#include "ui_loadView.h"
#include <QUrl>
#include <QWidget>

namespace Dragon
{

class LoadView : public QWidget, private Ui_LoadView
{
    Q_OBJECT
public:
    explicit LoadView(QWidget *parent);
    void setThumbnail(QWidget *object);
Q_SIGNALS:
    void loadUrl(const QUrl &);
    void openFilePressed();
    void openDVDPressed();
    void openStreamPressed();
    void reloadRecentlyList();
};

}
#endif
