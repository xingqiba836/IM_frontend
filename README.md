# IM_frontend

Qt 6 聊天客户端前端（llfcchat），包含登录/注册界面、HTTP 网关请求与微信风格 UI。

**作者：hexing**

## 环境

- Qt 6.x（Widgets + Network）
- MinGW 64-bit

## 配置

编辑运行目录下的 `config.ini`：

```ini
[GateServer]
host=localhost
port=8080
```

## 构建

使用 Qt Creator 打开 `chat_frontend.pro`，选择套件后构建运行。
