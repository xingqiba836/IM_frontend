#include "applyfriend.h"
#include "clickedbtn.h"
#include "customizeedit.h"
#include "global.h"
#include "tcpmgr.h"
#include "ui_applyfriend.h"
#include "usermgr.h"

#include <QDebug>
#include <QFontMetrics>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>
#include <QShowEvent>

#include <algorithm>

ApplyFriend::ApplyFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyFriend),_label_point(2,6)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Window);
    setObjectName(QStringLiteral("ApplyFriend"));
    setModal(true);
    ui->name_ed->setPlaceholderText(tr("恋恋风辰"));
    ui->lb_ed->setPlaceholderText("搜索、添加标签");
    ui->back_ed->setPlaceholderText("燃烧的胸毛");

	ui->lb_ed->SetMaxLength(21);
	ui->lb_ed->move(2, 2);
	ui->lb_ed->setFixedHeight(20);
	ui->lb_ed->setMaxLength(10);
	ui->input_tip_wid->hide();

    _tip_cur_point = QPoint(5, 5);

	_tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
							 "父与子学Python","nodejs开发指南","go 语言开发指南",
								"游戏伙伴","金融投资","微信读书","拼多多拼友" };

    connect(ui->more_lb, &ClickedOnceLabel::clicked, this, &ApplyFriend::ShowMoreLabel);
    InitTipLbs();
    //链接输入标签回车事件
    connect(ui->lb_ed, &CustomizeEdit::returnPressed, this, &ApplyFriend::SlotLabelEnter);
    connect(ui->lb_ed, &CustomizeEdit::textChanged, this, &ApplyFriend::SlotLabelTextChange);
    connect(ui->lb_ed, &CustomizeEdit::editingFinished, this, &ApplyFriend::SlotLabelEditFinished);
    connect(ui->tip_lb, &ClickedOnceLabel::clicked, this, &ApplyFriend::SlotAddFirendLabelByClickTip);

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->sure_btn->SetState("normal","hover","press");
    ui->cancel_btn->SetState("normal","hover","press");
    ui->cancel_btn->setFocusPolicy(Qt::NoFocus);
    ui->sure_btn->setFocusPolicy(Qt::NoFocus);
    connect(ui->cancel_btn, &ClickedBtn::clicked, this, &ApplyFriend::SlotApplyCancel);
    connect(ui->sure_btn, &ClickedBtn::clicked, this, &ApplyFriend::SlotApplySure);
    ui->lb_ed->setFocusPolicy(Qt::ClickFocus);
    updateGridWidgetSize();
}

ApplyFriend::~ApplyFriend()
{
    qDebug()<< "ApplyFriend destruct";
    delete ui;
}

void ApplyFriend::InitTipLbs()
{
    int lines = 1;
    for(int i = 0; i < _tip_data.size(); i++){

		auto* lb = new ClickedLabel(ui->lb_list);
		lb->SetState("normal", "hover", "pressed", "selected_normal",
			"selected_hover", "selected_pressed");
		lb->setObjectName("tipslb");
		lb->setText(_tip_data[i]);
		connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

		QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
		int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
		int textHeight = fontMetrics.height(); // 获取文本的高度

		if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            lines++;
            if (lines > 2) {
                delete lb;
                return;
            }
			
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);

		}

       auto next_point = _tip_cur_point;

       AddTipLbs(lb, _tip_cur_point,next_point, textWidth, textHeight);

       _tip_cur_point = next_point;
    }

}


void ApplyFriend::AddTipLbs(ClickedLabel *lb,
                           QPoint cur_point,
                           QPoint &next_point,
                           int text_width,
                           int text_height)
{
    Q_UNUSED(text_height);
	lb->move(cur_point);
	lb->show();
	_add_labels.insert(lb->text(), lb);
	_add_label_keys.push_back(lb->text());
	next_point.setX(lb->pos().x() + text_width + 15);
    next_point.setY(lb->pos().y());
}

bool ApplyFriend::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->scrollArea && event->type() == QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (obj == ui->scrollArea && event->type() == QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void ApplyFriend::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    updateGridWidgetSize();
    if (!_friend_labels.isEmpty()) {
        _label_point = QPoint(2, 6);
        resetLabels();
    }
}

int ApplyFriend::effectiveGridWidth() const
{
    int width = ui->gridWidget->width();
    if (width < MIN_APPLY_LABEL_ED_LEN) {
        width = ui->apply_wid->width() - 8;
    }
    if (width < MIN_APPLY_LABEL_ED_LEN) {
        width = 310;
    }
    return width;
}

void ApplyFriend::updateGridWidgetSize()
{
    const int gridWidth = effectiveGridWidth();
    ui->gridWidget->setMinimumWidth(gridWidth);
    ui->gridWidget->resize(gridWidth, qMax(ui->gridWidget->height(), 40));
}

void ApplyFriend::SetSearchInfo(const std::shared_ptr<SearchInfo> &si)
{
    _si = si;
    if (!si) {
        return;
    }

    QString applyname = UserMgr::GetInstance()->GetName();
    if (applyname.isEmpty()) {
        applyname = QStringLiteral("hexing");
    }

    ui->name_ed->setText(QStringLiteral("您好,我是%1").arg(applyname));
    ui->back_ed->setText(si->_name);
}

void ApplyFriend::ShowMoreLabel()
{
    qDebug()<< "receive more label clicked";
    ui->more_lb_wid->hide();

    ui->lb_list->setFixedWidth(325);
    _tip_cur_point = QPoint(5, 5);
    auto next_point = _tip_cur_point;
    int textWidth;
    int textHeight;
    //重拍现有的label
    for(auto & added_key : _add_label_keys){
        auto added_lb = _add_labels[added_key];

        QFontMetrics fontMetrics(added_lb->font()); // 获取QLabel控件的字体信息
        textWidth = fontMetrics.horizontalAdvance(added_lb->text()); // 获取文本的宽度
        textHeight = fontMetrics.height(); // 获取文本的高度

        if(_tip_cur_point.x() +textWidth + tip_offset > ui->lb_list->width()){
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y()+textHeight+15);
        }
        added_lb->move(_tip_cur_point);

        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(_tip_cur_point.y());

        _tip_cur_point = next_point;

    }

    //添加未添加的
    for(int i = 0; i < _tip_data.size(); i++){
        auto iter = _add_labels.find(_tip_data[i]);
        if(iter != _add_labels.end()){
            continue;
        }

		auto* lb = new ClickedLabel(ui->lb_list);
		lb->SetState("normal", "hover", "pressed", "selected_normal",
			"selected_hover", "selected_pressed");
		lb->setObjectName("tipslb");
		lb->setText(_tip_data[i]);
		connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

		QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
		int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
		int textHeight = fontMetrics.height(); // 获取文本的高度

		if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {

			_tip_cur_point.setX(tip_offset);
			_tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);

		}

		 next_point = _tip_cur_point;

		AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);

		_tip_cur_point = next_point;

    }

   int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
   ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    //qDebug()<<"after resize ui->lb_list size is " <<  ui->lb_list->size();
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height()+diff_height);
}

void ApplyFriend::resetLabels()
{
    const int max_width = effectiveGridWidth();
    auto label_height = 0;
    for(auto iter = _friend_labels.begin(); iter != _friend_labels.end(); iter++){
        if( _label_point.x() + iter.value()->width() > max_width) {
            _label_point.setY(_label_point.y()+iter.value()->height()+6);
            _label_point.setX(2);
        }

        iter.value()->move(_label_point);
        iter.value()->show();
        iter.value()->raise();

        _label_point.setX(_label_point.x()+iter.value()->width()+2);
        _label_point.setY(_label_point.y());
        label_height = iter.value()->height();
    }

    if(_friend_labels.isEmpty()){
         ui->lb_ed->move(_label_point);
         return;
    }

    if(_label_point.x() + MIN_APPLY_LABEL_ED_LEN > effectiveGridWidth()){
        ui->lb_ed->move(2,_label_point.y()+label_height+6);
    }else{
         ui->lb_ed->move(_label_point);
    }
}

void ApplyFriend::addLabel(const QString &name)
{
    if (_friend_labels.find(name) != _friend_labels.end()) {
        ui->lb_ed->clear();
        return;
    }

	auto tmplabel = new FriendLabel(ui->gridWidget);
	tmplabel->SetText(name);
	tmplabel->setObjectName("FriendLabel");

	const int max_width = effectiveGridWidth();
	if (_label_point.x() + tmplabel->width() > max_width) {
		_label_point.setY(_label_point.y() + tmplabel->height() + 6);
		_label_point.setX(2);
	}

	tmplabel->move(_label_point);
	tmplabel->show();
	tmplabel->raise();
	_friend_labels[tmplabel->Text()] = tmplabel;
	_friend_label_keys.push_back(tmplabel->Text());

	connect(tmplabel, &FriendLabel::sig_close, this, &ApplyFriend::SlotRemoveFriendLabel);

	_label_point.setX(_label_point.x() + tmplabel->width() + 2);

	if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > effectiveGridWidth()) {
		ui->lb_ed->move(2, _label_point.y() + tmplabel->height() + 2);
	}
	else {
		ui->lb_ed->move(_label_point);
	}

	ui->lb_ed->clear();

	if (ui->gridWidget->minimumHeight() < _label_point.y() + tmplabel->height() + 6) {
		ui->gridWidget->setMinimumHeight(_label_point.y() + tmplabel->height() + 6);
	}
}

void ApplyFriend::SlotLabelEnter()
{
    if(ui->lb_ed->text().isEmpty()){
        return;
    }

    auto text = ui->lb_ed->text();

    addLabel(ui->lb_ed->text());

    ui->input_tip_wid->hide();

    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    //找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end()) {
        _tip_data.push_back(text);
    }

    //判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }

    //标签展示栏也增加一个标签, 并设置绿色选中
    auto* lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed", "selected_normal",
        "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

    if (_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width()) {

        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);

    }

    auto next_point = _tip_cur_point;

    AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
    _tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    lb->SetCurState(ClickLbState::Selected);

    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void ApplyFriend::SlotRemoveFriendLabel(const QString &name)
{
    qDebug() << "receive close signal";

    _label_point.setX(2);
    _label_point.setY(6);

   auto find_iter = _friend_labels.find(name);

   if(find_iter == _friend_labels.end()){
       return;
   }

   auto find_key = _friend_label_keys.end();
   for(auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end();
       iter++){
       if(*iter == name){
           find_key = iter;
           break;
       }
   }

   if(find_key != _friend_label_keys.end()){
      _friend_label_keys.erase(find_key);
   }


   delete find_iter.value();

   _friend_labels.erase(find_iter);

   resetLabels();

   auto find_add = _add_labels.find(name);
   if(find_add == _add_labels.end()){
        return;
   }

   find_add.value()->ResetNormalState();
}

//点击标已有签添加或删除新联系人的标签
void ApplyFriend::SlotChangeFriendLabelByTip(const QString &lbtext, ClickLbState state)
{
    auto find_iter = _add_labels.find(lbtext);
    if(find_iter == _add_labels.end()){
        return;
    }

    if(state == ClickLbState::Selected){
        //编写添加逻辑
        addLabel(lbtext);
        return;
    }

    if(state == ClickLbState::Normal){
        //编写删除逻辑
        SlotRemoveFriendLabel(lbtext);
        return;
    }

}

void ApplyFriend::SlotLabelTextChange(const QString& text)
{
    if (text.isEmpty()) {
        ui->tip_lb->setText("");
        ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(_tip_data.begin(), _tip_data.end(), text);
    if (iter == _tip_data.end()) {
        auto new_text = add_prefix + text;
        ui->tip_lb->setText(new_text);
        ui->input_tip_wid->show();
        return;
    }
    ui->tip_lb->setText(text);
    ui->input_tip_wid->show();
}

void ApplyFriend::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void ApplyFriend::SlotAddFirendLabelByClickTip(const QString &text)
{
    QString labelText = text;
    int index = labelText.indexOf(add_prefix);
    if (index != -1) {
        labelText = labelText.mid(index + add_prefix.length());
    }
    addLabel(labelText);

    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), labelText);
    //找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end()) {
        _tip_data.push_back(labelText);
    }
   
    //判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(labelText);
    if (find_add != _add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }
     
    //标签展示栏也增加一个标签, 并设置绿色选中
	auto* lb = new ClickedLabel(ui->lb_list);
	lb->SetState("normal", "hover", "pressed", "selected_normal",
		"selected_hover", "selected_pressed");
	lb->setObjectName("tipslb");
	lb->setText(labelText);
	connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();
   
	QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
	int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
	int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

	if (_tip_cur_point.x() + textWidth+ tip_offset+3 > ui->lb_list->width()) {

		_tip_cur_point.setX(5);
		_tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);

	}

	auto next_point = _tip_cur_point;

	 AddTipLbs(lb, _tip_cur_point, next_point, textWidth,textHeight);
	_tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    lb->SetCurState(ClickLbState::Selected);

    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height()+ diff_height );
}

void ApplyFriend::SlotApplySure()
{
    if (_closing) {
        return;
    }
    _closing = true;

    qDebug() << "Slot Apply Sure called";
    ui->lb_ed->clearFocus();
    if (_si) {
        QJsonObject jsonObj;
        jsonObj["uid"] = UserMgr::GetInstance()->GetUid();
        jsonObj["applyname"] = ui->name_ed->text().isEmpty()
                                   ? ui->name_ed->placeholderText()
                                   : ui->name_ed->text();
        jsonObj["bakname"] = ui->back_ed->text().isEmpty()
                                 ? ui->back_ed->placeholderText()
                                 : ui->back_ed->text();
        jsonObj["touid"] = _si->_uid;

        const QJsonDocument doc(jsonObj);
        emit TcpMgr::GetInstance()->sig_send_data(
            ReqId::ID_ADD_FRIEND_REQ,
            doc.toJson(QJsonDocument::Compact));
    }
    accept();
}

void ApplyFriend::SlotApplyCancel()
{
    if (_closing) {
        return;
    }
    _closing = true;

    qDebug() << "Slot Apply Cancel";
    ui->lb_ed->clearFocus();
    reject();
}


