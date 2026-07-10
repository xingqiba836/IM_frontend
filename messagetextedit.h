/******************************************************************************
 *
 * @file       messagetextedit.h
 * @brief      Rich message input editor
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include "global.h"

#include <QTextEdit>
#include <QVector>

class MessageTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit MessageTextEdit(QWidget *parent = nullptr);
    ~MessageTextEdit() override;

    QVector<MsgInfo> getMsgList();
    void insertFileFromUrl(const QStringList &urls);

signals:
    void send();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void insertFromMimeData(const QMimeData *source) override;

private:
    void insertImages(const QString &url);
    void insertTextFile(const QString &url);
    bool isImage(const QString &url);
    void insertMsgList(QVector<MsgInfo> &list, const QString &flag, const QString &text, const QPixmap &pix);
    QStringList getUrl(const QString &text);
    QPixmap getFileIconPixmap(const QString &url);
    QString getFileSize(qint64 size);

private slots:
    void textEditChanged();

private:
    QVector<MsgInfo> mMsgList;
    QVector<MsgInfo> mGetMsgList;
};

#endif // MESSAGETEXTEDIT_H
