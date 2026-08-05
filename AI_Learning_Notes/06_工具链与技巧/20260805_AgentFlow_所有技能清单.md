# AgentFlow 全部已安装技能清单

> 来源：AgentFlow MCP `list_skills()` | 获取时间 2026-08-05 | 共 13 个技能

---

## UE5 相关（2 个，重点）

| # | skill_type | 名称 | 版本 | 最后进化 | 知识条数 |
|---|-----------|------|------|---------|---------|
| 1 | **ue5_character** | UE5 角色系统 | v8 | 2026-04-21 | 6 条 best_practices |
| 2 | **ue5_cpp_teaching** | UE5 C++ 教学 | v5 | 2026-04-25 | 7 条 best_practices |

### ue5_character 核心知识点
- Enhanced Input + AnimMontage 攻击完整链路（5 环缺一不可）
- ObjectInitializer 替换默认组件的标准写法（对标 Lyra ALyraCharacter）
- Constructor 里修改共享 DataAsset 属性会污染资产
- 热重载时 FObjectFinder 找不到 DataAsset → 忽略即可
- 蒙太奇播放无效果 → 检查 DefaultSlot 节点
- P1 合并：热重载警告 + DataAsset 污染陷阱综合

### ue5_cpp_teaching 核心知识点
- 三种编译方式教学（Ctrl+Alt+F11 / 菜单 / 重启）
- .h 和 .cpp 变量名必须完全一致
- 子文件夹组织代码前必须先改 Build.cs 加 PublicIncludePaths
- 全程 C++ 教学时用 ConstructorHelpers 加载资源
- UE5 资产迁移精准筛选原则（最小依赖 SOP，控制在 10 个文件内）
- 学生烦躁时的精简三步法
- 动画资产类型区分（Sequence/Montage/Pose Asset/Blueprint）

---

## Cocos Creator 相关（3 个）

| # | skill_type | 名称 | 版本 | 最后进化 | 知识条数 |
|---|-----------|------|------|---------|---------|
| 3 | cocos_mcp_server | Cocos MCP 服务器 | v19 | 2026-04-04 | 13 条 best_practices |
| 4 | cocos_prefab_tools | Cocos Prefab 工具 | v3 | 2026-04-17 | 5 条 best_practices |
| 5 | psb_to_prefab | PSB→Prefab 转换器 | v17 | 2026-04-15 | 12 rules + 10 anti_patterns + 12 best_practices |

### cocos_mcp_server 核心知识点
- ToolManager 持久化配置系统
- HTTP MCP 服务器实现（JSON-RPC 2.0）
- Prefab 创建 7 步流程
- propertyType 15 种类型枚举模式
- 14 类 57+ 工具完整清单
- 12 条反模式规则集（RULE-1 到 RULE-12）
- 4 个严重 Bug（Color/UITransform/组件引用/db://路径）
- 3 个严重 Bug（query-node 解包/2D z 轴/时序）

### psb_to_prefab 核心知识点
- 12 条规则（UUID 引用、PrefabInfo、meta 格式等）
- 10 条反模式（Texture UUID 误用、TRIMMED 覆盖、__id__ 引用等）
- UUID 压缩算法：固定 b64[:-2] 截断
- 编辑器缓存三层架构排查
- 端到端工作流 7 步

---

## 前端开发相关（1 个）

| # | skill_type | 名称 | 版本 | 最后进化 | 知识条数 |
|---|-----------|------|------|---------|---------|
| 6 | frontend_vue | Vue 3 前端开发 | v5 | 2026-04-07 | 14 条 best_practices |

### frontend_vue 核心知识点
- Vue 3 + TypeScript 类型系统最佳实践
- JS→TS 迁移标准流程
- 截图分析→Vue 页面生成 4 步流程
- 纯 CSS 绘制游戏风格 UI 元素技巧
- 入场动画序列编排模式
- CSS 粒子系统实现模式
- 游戏弹窗页面通用结构模板

---

## 蓝湖集成相关（1 个）

| # | skill_type | 名称 | 版本 | 最后进化 | 知识条数 |
|---|-----------|------|------|---------|---------|
| 7 | lanhu_mcp | 蓝湖 MCP | v5 | 2026-04-03 | 10 条 best_practices |

### lanhu_mcp 核心知识点
- 蓝湖 MCP 服务端配置与客户端连接
- URL 类型判断技巧（设计图 vs PRD）
- 设计图转 Vue 3 组件的代码生成模式
- 6 步标准工作流（Design-to-Code SOP）
- Schema 获取 4 步 API 调用链
- Hash 路由 URL 解析模式
- CDN 下载地址直接获取

---

## 预置技能（6 个）

| # | skill_type | 名称 | 版本 | 知识条数 |
|---|-----------|------|------|---------|
| 8 | api_design | API 设计 | v1 | 4 rules + 2 anti + 3 bp |
| 9 | auth_security | 认证安全 | v1 | 4 rules + 3 anti + 3 bp |
| 10 | code_review | 代码审查 | v1 | 3 rules + 2 anti + 3 bp |
| 11 | db_storage | 数据库存储 | v1 | 4 rules + 3 anti + 3 bp |
| 12 | go_crud | Go CRUD | v1 | 5 rules + 3 anti + 3 bp |
| 13 | testing | 测试 | v1 | 4 rules + 2 anti + 3 bp |

---

## 经验数据

- 经验搜索返回空（workspace 未关联到 AgentFlow）
- SQLite 上下文指标不可用
- 之前记忆中提到有 245 条正向 + 87 条负向经验，但当前 workspace 无法访问
