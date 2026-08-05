# AgentFlow MCP 连接问题

## 问题描述

AgentFlow MCP 服务器在运行（端口 9080 监听），但 API 调用返回空数据。

## 排查过程

1. ✅ 确认端口 9080 有进程监听
2. ✅ 确认 AgentFlow v2.0.0 在运行
3. ✅ 发现 MCP 暴露了 92 个工具
4. ❌ list_goals 返回空数组
5. ❌ get_dashboard 返回空结果
6. ❌ list_projects 返回空列表

## 根因

Workspace 未关联到 AgentFlow。MCP 服务器虽然在运行，但当前项目目录 `e:\ue5\ue5_battle` 没有被 AgentFlow 识别为工作区。

## 解决方案

需要在 AgentFlow 中将此项目添加为 workspace，或者通过 API 正确设置 workspace context。

## PowerShell JSON-RPC 发送经验

多次尝试后发现最可靠的方式是用 Python：

```python
import urllib.request, json
req = urllib.request.Request(
    'http://127.0.0.1:9080/mcp',
    data=json.dumps({'jsonrpc':'2.0','method':'list_goals','id':1,'params':{'page_size':50}}).encode('utf-8'),
    headers={'Content-Type':'application/json'}
)
resp = urllib.request.urlopen(req)
print(resp.read().decode('utf-8'))
```

PowerShell 的痛点：
- `[System.Text.Encoding]::UTF8.GetBytes($json)` 中的双引号会被转义
- `Set-Content -Encoding UTF8` 会写入 BOM 导致 JSON 解析失败
- `WebClient.UploadString` 编码不可控
- 最终 `WebClient.UploadData` + 无 BOM UTF8 也失败了

**结论**：涉及复杂 JSON-RPC 请求时直接用 Python，别跟 PowerShell 较劲。
