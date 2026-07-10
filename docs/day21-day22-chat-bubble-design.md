# Day21 / Day22 聊天气泡模块设计说明

> 对应教程：滚动聊天布局（Day21）、气泡聊天对话框（Day22）  
> 参考源码：[llfcchat/client/llfcchat](https://github.com/secondtonone1/llfcchat/tree/master/client/llfcchat)

---

## 1. 设计目标

实现类似微信的聊天体验：

- 中间区域可滚动展示历史消息
- 自己发送的消息：绿色气泡 + 右对齐 + 头像在右
- 对方消息：白色气泡 + 左对齐 + 头像在左
- 支持纯文本、图片（拖拽/粘贴）混合输入
- Enter 发送，Shift+Enter 换行

当前阶段为 **本地 UI 演示**，尚未接入 TCP 网络发送。

---

## 2. 界面结构（ChatPage）

`chatpage.ui` 自上而下：

```
┌─────────────────────────────────┐
│ chat_title                      │  标题栏（头像 + 聊天对象名）
├─────────────────────────────────┤
│ chat_data_list (ChatView)       │  消息展示区（可滚动）
├─────────────────────────────────┤
│ tool_wid                        │  工具栏（表情、文件按钮）
├─────────────────────────────────┤
│ chatEdit (MessageTextEdit)      │  输入区
├─────────────────────────────────┤
│ send_wid                        │  接收 / 发送 按钮
└─────────────────────────────────┘
```

### 与 Day20 的差异

| 控件 objectName | Day20 | Day21/22 |
|-----------------|-------|----------|
| 中间展示区 | `chatEdit`（只读 QTextEdit） | `chat_data_list`（ChatView） |
| 底部输入 | `chat_input`（QTextEdit） | `chatEdit`（MessageTextEdit） |

---

## 3. 代码分层

```
┌─────────────────────────────────────────────────────────┐
│  ChatPage（总控）                                         │
│  · 连接信号                                               │
│  · getMsgList() → appendMsgList()                        │
└────────────┬───────────────────────────┬────────────────┘
             │                           │
             ▼                           ▼
┌────────────────────────┐   ┌────────────────────────────┐
│ MessageTextEdit        │   │ ChatView                   │
│ 输入 → MsgInfo 列表     │   │ 滚动容器 → appendChatItem  │
└────────────────────────┘   └─────────────┬──────────────┘
                                           │
                                           ▼
                             ┌────────────────────────────┐
                             │ ChatItemBase（一行消息）      │
                             │  昵称 + 头像 + 气泡           │
                             └─────────────┬──────────────┘
                                           │
                                           ▼
                             ┌────────────────────────────┐
                             │ BubbleFrame                │
                             │  ├── TextBubble（文本）     │
                             │  └── PictureBubble（图片）  │
                             └────────────────────────────┘
```

### 文件职责一览

| 文件 | 职责 |
|------|------|
| `global.h` | `ChatRole`（Self/Other）、`MsgInfo`（msgFlag/content/pixmap） |
| `messagetextedit.*` | 富文本输入、拖拽、Enter 发送、`getMsgList()` 解析 |
| `chatview.*` | 滚动区域、消息追加、自动滚到底、悬停显示滚动条 |
| `chatitembase.*` | 单行消息布局（Grid：头像、昵称、气泡位置） |
| `bubbleframe.*` | 气泡背景 + 三角，`paintEvent` 绘制 |
| `textbubble.*` | 文本气泡，自适应宽高 |
| `picturebubble.*` | 图片气泡，限定最大尺寸 |
| `chatpage.*` | 组装上述模块，处理发送/接收 |

---

## 4. 核心数据结构

定义于 `global.h`：

```cpp
enum class ChatRole {
    Self,   // 自己发的，右对齐，绿色气泡
    Other   // 对方发的，左对齐，白色气泡
};

struct MsgInfo {
    QString msgFlag;   // "text" | "image" | "file"
    QString content;   // 文本内容，或图片/文件路径
    QPixmap pixmap;    // 图片/文件缩略图
};
```

---

## 5. 发送/接收完整链路

### 5.1 流程图

```
用户输入（MessageTextEdit）
        │
        ▼ 点击发送 / Enter
ChatPage::on_send_btn_clicked()
        │
        ▼ ui->chatEdit->getMsgList()
QVector<MsgInfo> msgList
        │
        ▼ appendMsgList(msgList, role, userName, userIcon)
循环每条 MsgInfo：
  · new ChatItemBase(role)
  · setUserName / setUserIcon
  · new TextBubble 或 PictureBubble
  · pChatItem->setWidget(pBubble)
  · ui->chat_data_list->appendChatItem(pChatItem)
        │
        ▼ Qt 布局系统自动 show + 重绘
ChatView 滚动到底部（onVScrollBarMoved）
```

### 5.2 关键代码（ChatPage）

```cpp
void ChatPage::on_send_btn_clicked()
{
    const ChatRole role = ChatRole::Self;
    const QString userName = QStringLiteral("hexing");
    const QString userIcon = QStringLiteral(":/res/head_1.jpg");
    const QVector<MsgInfo> msgList = ui->chatEdit->getMsgList();  // ← 输入端
    appendMsgList(msgList, role, userName, userIcon);             // ← 展示端
}
```

「接收」按钮逻辑相同，仅 `role = Other`、用户名/头像换成模拟对方（zack）。

### 5.3 三种触发发送的方式

均在 `ChatPage` 构造函数中 connect：

1. 点击 `send_btn` → `on_send_btn_clicked`
2. 点击 `receive_btn` → `on_receive_btn_clicked`
3. 输入框按 Enter → `MessageTextEdit::send` → `on_send_btn_clicked`

---

## 6. MessageTextEdit 设计

### 6.1 角色定位

**输入端的数据生产者**，不参与消息展示。

- `ui->chatEdit` 在 UI 里是底部输入框
- 发送前调用 `getMsgList()`，把编辑器内容解析为 `QVector<MsgInfo>`
- 解析完成后 `clear()` 清空输入框

### 6.2 getMsgList() 解析逻辑

遍历 `document()->toPlainText()`：

| 字符类型 | 处理方式 |
|----------|----------|
| 普通字符 | 累加到 `text` 缓冲区 |
| `ObjectReplacementCharacter`（U+FFFC） | 图片/文件占位符；先把前面累积的文字作为 `text` 消息；再从 `mMsgList` 匹配对应的 `image`/`file` |

示例：输入 `"你好"` + 拖入一张图 + 输入 `"再见"`

```
getMsgList() 返回：
  [0] { msgFlag: "text",  content: "你好" }
  [1] { msgFlag: "image", content: "/path/to/pic.png" }
  [2] { msgFlag: "text",  content: "再见" }
```

### 6.3 插入图片时的双轨记录

`insertImages()` 做两件事：

1. `cursor.insertImage(...)` — 在编辑器里显示缩略图（占位符）
2. `insertMsgList(mMsgList, "image", url, pixmap)` — 在 `mMsgList` 里记录元数据

发送时靠占位符 + `mMsgList` 对应，还原出结构化消息。

### 6.4 快捷键

- **Enter**：`emit send()`，不插入换行
- **Shift+Enter**：正常换行（`QTextEdit::keyPressEvent`）

---

## 7. ChatView 设计（Day21）

### 7.1 内部布局

```
ChatView
 └── QVBoxLayout
      └── QScrollArea (objectName: chat_area)
           ├── 内部 widget (objectName: chat_bg)
           │    └── QVBoxLayout
           │         ├── [弹簧 widget, stretch=100000]  ← 把消息顶到上方
           │         ├── ChatItemBase（消息1）
           │         ├── ChatItemBase（消息2）
           │         └── ...
           └── QHBoxLayout（滚动条浮在右侧）
```

底部大 stretch 弹簧的作用：消息少时从**上方**开始排列（类似微信），而不是堆在底部。

### 7.2 appendChatItem

```cpp
vl->insertWidget(vl->count() - 1, item);
```

始终插在**倒数第二个**位置（弹簧前面），实现尾插。

### 7.3 自动滚到底

1. `appendChatItem` 设置 `isAppended = true`
2. 内容增高触发 `QScrollBar::rangeChanged`
3. `onVScrollBarMoved` 把滚动条设为 `maximum`
4. 500ms 后重置 `isAppended`（防止连续追加时重复触发）

### 7.4 滚动条显隐

`eventFilter` 监听 `QScrollArea`：

- 鼠标进入 → 有内容则显示滚动条
- 鼠标离开 → 隐藏滚动条

---

## 8. ChatItemBase 设计

一行消息 = **昵称 + 头像 + 气泡**，使用 `QGridLayout`。

### Self（自己，靠右）

```
[  弹簧  ][ 名字 ][ 头像 ]
         [ 气泡  ]
```

### Other（对方，靠左）

```
[ 头像 ][ 名字 ][  弹簧  ]
       [ 气泡  ]
```

### setWidget

构造时 `m_pBubble` 是占位空 widget；设置真实气泡时用 `replaceWidget` 替换：

```cpp
pGLayout->replaceWidget(m_pBubble, widget);
delete m_pBubble;
m_pBubble = widget;
```

---

## 9. 气泡继承体系

```
QFrame
  └── BubbleFrame          paintEvent 绘制圆角矩形 + 小三角
        ├── TextBubble     内嵌 QTextEdit，Paint 时自适应高度
        └── PictureBubble  内嵌 QLabel 显示图片
```

### BubbleFrame 配色

| ChatRole | 背景色 | 三角位置 |
|----------|--------|----------|
| Self | RGB(158, 234, 106) 绿色 | 气泡右侧 |
| Other | 白色 | 气泡左侧 |

三角宽度常量：`WIDTH_SANJIAO = 8`。

### TextBubble 自适应

- **宽度**：遍历每行文本，取最宽一行 + 边距 → `setMaximumWidth`
- **高度**：在 `eventFilter` 监听 `QEvent::Paint`，累加各行高度 → `setFixedHeight`

---

## 10. appendMsgList 之后如何显示到界面

`appendMsgList` **不负责上传网络**，只做本地控件组装：

1. `new ChatItemBase` + `new TextBubble/PictureBubble` — 在内存中创建控件树
2. `ui->chat_data_list->appendChatItem(pChatItem)` — 挂到 ChatView 的 QVBoxLayout
3. Qt 布局引擎自动计算位置、触发 `paintEvent` 绘制
4. ChatView 滚到最新消息

**没有单独的「刷新界面」调用**，往 layout 插入 widget 即完成显示。

---

## 11. 与 llfcchat 完整版的差异

| 功能 | 当前 IM_frontend | llfcchat 完整版 |
|------|------------------|-----------------|
| TCP 发送消息 | 未接入 | `TcpMgr::sig_send_data(ID_TEXT_CHAT_MSG_REQ, ...)` |
| 消息送达状态 | 无 | `ChatItemBase::setStatus()` + unread 图标 |
| 多会话 / 好友 | 写死 hexing/zack | `UserMgr` + 好友列表 |
| 文件气泡展示 | 输入支持，展示未实现 | 完整 PictureBubble / 文件 UI |
| 表情按钮 | UI 已有，逻辑未接 | 后续章节 |

---

## 12. 扩展方向（后续 Day）

- Day23+：侧边栏切换、搜索联动
- 接入 `UserMgr` 获取真实用户名/头像
- `on_send_btn_clicked` 中增加 TCP 发包
- 收到服务端消息后，以 `ChatRole::Other` 调用 `appendMsgList`
- 文件类型气泡、`emo_lb` 表情选择

---

## 13. 相关源文件索引

```
chat_frontend/
├── global.h                 # ChatRole, MsgInfo
├── chatpage.ui / .h / .cpp  # 页面总控
├── messagetextedit.h / .cpp # 输入解析
├── chatview.h / .cpp        # 滚动列表
├── chatitembase.h / .cpp    # 单行布局
├── bubbleframe.h / .cpp     # 气泡基类
├── textbubble.h / .cpp      # 文本气泡
├── picturebubble.h / .cpp   # 图片气泡
└── style/stylesheet.qss     # #chat_area, #chat_bg, #chatEdit 等样式
```
