/******************************************************************************
 *
 * @file       customizeedit.h
 * @brief      Search line edit with length limit
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QLineEdit>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomizeEdit(QWidget *parent = nullptr);

    void SetMaxLength(int maxLen);

signals:
    void sig_foucus_out();

protected:
    void focusOutEvent(QFocusEvent *event) override;

private:
    void limitTextLength(const QString &text);

    int _max_len;
};

#endif // CUSTOMIZEEDIT_H
