/******************************************************************************
 *
 * @file       messagetextedit.cpp
 * @brief      Rich message input editor
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "messagetextedit.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QFontMetrics>
#include <QImage>
#include <QTextCursor>

MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setMaximumHeight(120);
}

MessageTextEdit::~MessageTextEdit() = default;

QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();

    const QString doc = document()->toPlainText();
    QString text;
    int indexUrl = 0;
    const int count = mMsgList.size();

    for (int index = 0; index < doc.size(); ++index) {
        if (doc[index] == QChar::ObjectReplacementCharacter) {
            if (!text.isEmpty()) {
                insertMsgList(mGetMsgList, QStringLiteral("text"), text, QPixmap());
                text.clear();
            }
            while (indexUrl < count) {
                const MsgInfo msg = mMsgList.at(indexUrl);
                if (document()->toHtml().contains(msg.content, Qt::CaseSensitive)) {
                    ++indexUrl;
                    mGetMsgList.append(msg);
                    break;
                }
                ++indexUrl;
            }
        } else {
            text.append(doc[index]);
        }
    }

    if (!text.isEmpty()) {
        insertMsgList(mGetMsgList, QStringLiteral("text"), text, QPixmap());
        text.clear();
    }

    mMsgList.clear();
    clear();
    return mGetMsgList;
}

void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->source() == this) {
        event->ignore();
    } else {
        event->accept();
    }
}

void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        && !(event->modifiers() & Qt::ShiftModifier)) {
        emit send();
        return;
    }
    QTextEdit::keyPressEvent(event);
}

void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if (urls.isEmpty()) {
        return;
    }

    for (const QString &url : urls) {
        if (isImage(url)) {
            insertImages(url);
        } else {
            insertTextFile(url);
        }
    }
}

void MessageTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    if (image.width() > 120 || image.height() > 80) {
        if (image.width() > image.height()) {
            image = image.scaledToWidth(120, Qt::SmoothTransformation);
        } else {
            image = image.scaledToHeight(80, Qt::SmoothTransformation);
        }
    }

    QTextCursor cursor = textCursor();
    cursor.insertImage(image, url);
    insertMsgList(mMsgList, QStringLiteral("image"), url, QPixmap::fromImage(image));
}

void MessageTextEdit::insertTextFile(const QString &url)
{
    const QFileInfo fileInfo(url);
    if (fileInfo.isDir()) {
        QMessageBox::information(this, tr("提示"), tr("只允许拖拽单个文件!"));
        return;
    }

    if (fileInfo.size() > 100 * 1024 * 1024) {
        QMessageBox::information(this, tr("提示"), tr("发送的文件大小不能大于100M"));
        return;
    }

    const QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = textCursor();
    cursor.insertImage(pix.toImage(), url);
    insertMsgList(mMsgList, QStringLiteral("file"), url, pix);
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    const QStringList urls = getUrl(source->text());
    if (urls.isEmpty()) {
        return;
    }

    for (const QString &url : urls) {
        if (isImage(url)) {
            insertImages(url);
        } else {
            insertTextFile(url);
        }
    }
}

bool MessageTextEdit::isImage(const QString &url)
{
    const QString imageFormat = QStringLiteral(
        "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp");
    const QStringList imageFormatList = imageFormat.split(',');
    const QFileInfo fileInfo(url);
    const QString suffix = fileInfo.suffix();
    return imageFormatList.contains(suffix, Qt::CaseInsensitive);
}

void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list,
                                    const QString &flag,
                                    const QString &text,
                                    const QPixmap &pix)
{
    MsgInfo msg;
    msg.msgFlag = flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}

QStringList MessageTextEdit::getUrl(const QString &text)
{
    QStringList urls;
    if (text.isEmpty()) {
        return urls;
    }

    const QStringList list = text.split('\n');
    for (const QString &url : list) {
        if (url.isEmpty()) {
            continue;
        }
        const QStringList str = url.split(QStringLiteral("///"));
        if (str.size() >= 2) {
            urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provider;
    const QFileInfo fileInfo(url);
    const QIcon icon = provider.icon(fileInfo);
    const QString strFileSize = getFileSize(fileInfo.size());

    QFont font(QStringLiteral("宋体"), 10, QFont::Normal, false);
    QFontMetrics fontMetrics(font);
    const QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileInfo.fileName());
    const QSize fileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    const int maxWidth = textSize.width() > fileSize.width() ? textSize.width() : fileSize.width();

    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    painter.begin(&pix);
    const QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40, 40));
    painter.setPen(Qt::black);
    const QRect rectText(50 + 10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileInfo.fileName());
    const QRect rectFile(50 + 10, textSize.height() + 5, fileSize.width(), fileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MessageTextEdit::getFileSize(qint64 size)
{
    QString unit;
    double num = 0;
    if (size < 1024) {
        num = size;
        unit = QStringLiteral("B");
    } else if (size < 1024 * 1024) {
        num = size / 1024.0;
        unit = QStringLiteral("KB");
    } else if (size < 1024 * 1024 * 1024) {
        num = size / 1024.0 / 1024.0;
        unit = QStringLiteral("MB");
    } else {
        num = size / 1024.0 / 1024.0 / 1024.0;
        unit = QStringLiteral("GB");
    }
    return QString::number(num, 'f', 2) + QLatin1Char(' ') + unit;
}

void MessageTextEdit::textEditChanged()
{
}
