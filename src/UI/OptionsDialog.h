#pragma once

#include <QDialog>

#include "ui_OptionsDialog.h"

class OptionsDialog : public QDialog {
    Q_OBJECT
public:
    OptionsDialog(QWidget *parent = nullptr);
    virtual ~OptionsDialog() override;

private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();

private:
    Ui::OptionsDialog m_ui;
};
