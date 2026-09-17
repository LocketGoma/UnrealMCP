# UnrealMCP

## English

A plugin bundle that backports Unreal Engine 5.8 MCP functionality to UE 5.7.4. MCP-compatible AI clients can query editor state, work with asset and actor selections, control the viewport, and read or write editor logs.

### Compatibility

**Experimental/beta. UE 5.7.4 is the currently verified environment; builds and behavior on earlier versions are not guaranteed.**

This plugin is intended to enable UnrealMCP on Unreal Engine 5.7.4 and earlier versions. Running it on Unreal Engine 5.8 or later may cause unexpected issues. Please use the official MCP on Unreal Engine 5.8 or later.

### Included plugins

Install the subplugins together. `UnrealMCP` provides the MCP server, while `EditorToolset` provides editor tools. Plugin dependencies are declared in their `.uplugin` descriptors.

|Plugin |Purpose |
| --- | --- |
| UnrealMCP |MCP server and tool dispatch |
| EditorToolset |Editor state, selection, viewport, and log tools |
| ToolsetRegistry |Tool registration, discovery, and AgentSkill management |
| JsonUtilitiesEditor |Compatibility modules for JSON schema generation |
| FileSandbox |File sandboxing, ZIP handling, and shared compatibility code |

### Setup and connection

1. Place this repository under `Engine/Plugins/UnrealMCP/` in UE 5.7.4, preserving the folder layout below.
2. Enable `EditorToolset` and its required plugins, including `UnrealMCP`. This is a source plugin bundle: build it with a compatible C++ toolchain and restart the editor.
3. Check the port and path in the `Model Context Protocol` settings, enable `Auto Start Server`, and restart. Alternatively, launch the editor with `-ModelContextProtocolStartServer`.
4. Add `http://127.0.0.1:8000/mcp` as a **Streamable HTTP** server in your MCP client, adjusting the URL if configured differently.

```text
Engine/Plugins/UnrealMCP/
├── UnrealMCP/
├── EditorToolset/
├── ToolsetRegistry/
├── JsonUtilitiesEditor/
├── FileSandbox/
└── README.md
```

Server auto-start is disabled by default. The loopback URL above is for clients running on the same computer as the editor. After adding plugin functions, rebuild, restart, and refresh the client's tool discovery.

### Features and discovery

The MCP connection checked on 2026-09-17 exposed 30 tools across these three toolsets. Use live discovery as the authority: the list can vary with plugin versions and enabled plugins.

| Toolset |Count |Capabilities |
| --- | ---: | --- |
| `EditorToolset.EditorAppToolset` | 21 |Asset and actor selection, open assets, camera, coordinate conversion, image capture, CVar search, PIE control |
| `EditorToolset.LogsToolset` | 5 |Read and write logs, list categories, get and set verbosity |
| `ToolsetRegistry.AgentSkillToolset` | 4 |List, read, create, and update AgentSkills |

With `Enable Tool Search` enabled by default, tool details are discovered on demand. Any connected MCP client can use the same discovery and invocation flow:

1. `list_toolsets` — List toolsets and summaries.
2. `describe_toolset` — Read tools and input schemas for one toolset.
3. `call_tool` — Invoke a tool with its toolset name, tool name, and arguments.

Example arguments to `call_tool` for selected assets:

```json
{
  "toolset_name": "EditorToolset.EditorAppToolset",
  "tool_name": "GetSelectedAssets",
  "arguments": {}
}
```

Example arguments to write an editor log message:

```json
{
  "toolset_name": "EditorToolset.LogsToolset",
  "tool_name": "WriteLog",
  "arguments": { "message": "Hello from MCP" }
}
```

`WriteLog` is an addition in this backport. It writes multiline text under `LogMCPMessage` at `Display` verbosity; query results are not logged automatically. Check Output Log filters if a message is not visible.

### Behavior and limitations

#### Content Browser selection

Queries existing standard tabs 1–4, including inactive and sidebar tabs in the supported tab managers, without opening or activating them. Only the Project (legacy) source is queried; Fab and other active sources are skipped without switching. Duplicates, closed tabs, and the separate Content Drawer are excluded. Drawer exclusion matches the inspected UE 5.8 behavior.

Custom browser instances and browsers hosted outside the global or Level Editor tab managers can be missed. Collection order follows standard tab numbers rather than UE 5.8's browser creation order. Both selection queries and `SelectAssets` completion checks use this approach, which depends on the UE 5.7.4 widget structure.

#### Viewport and state queries

World/screen conversions use the first perspective level viewport, matching the UE 5.8 selection policy. The backport uses UE 5.7.4 projection APIs and returns failure with cleared outputs for an invalid viewport.

Two no-argument tools have been added to `EditorToolset.EditorAppToolset`: `GetEngineVersion` returns the running engine's full version string; `GetCurrentLevel` returns the editor world's map package path. The latter identifies the persistent map, not the active sublevel or PIE world; unsaved maps may have temporary paths. Rebuild and restart to use them. These two additions to the previously observed 30 tools have not yet been build- or runtime-validated.

For startup logs, explicitly set `category` to an empty string and filter with `pattern`: category filtering can omit startup lines without timestamps.

### Validation status

As of 2026-09-17:

- The development environment reported a successful UE 5.7.4 build.
- MCP connection, discovery, selected-asset queries, log reads, and writing/reading back a `WriteLog` message were verified in a live session.
- This does not establish coverage for every tool, all hidden-tab/Fab combinations, packaging, or earlier engine versions.

---

## 한국어

Unreal Engine 5.8의 MCP(Model Context Protocol) 기능을 UE 5.7.4 환경에 이식하는 플러그인 모음입니다. MCP를 지원하는 AI 클라이언트에서 에디터 상태를 조회하고, 에셋·액터 선택, 뷰포트 조작, 로그 확인 등의 작업을 수행할 수 있습니다.

### 지원 범위

**실험적·베타 단계입니다. 현재 확인된 환경은 UE 5.7.4이며, 그보다 낮은 버전의 빌드·동작은 보장하지 않습니다.**

해당 플러그인은 5.7.4 이하의 버전에서 UnrealMCP를 작동시키기 위한 플러그인으로, 5.8 이후 버전에서 실행 시 예기치못한 문제가 발생할 수 있습니다. 5.8 이후 버전에서는 공식 MCP를 이용해주세요.

### 구성

하위 플러그인을 함께 설치하는 구성입니다. `UnrealMCP`가 MCP 서버를 제공하고, `EditorToolset`이 에디터용 도구를 제공합니다. 필요한 플러그인 간 의존성은 각 `.uplugin`에 선언되어 있습니다.

| 플러그인| 역할|
| --- | --- |
| UnrealMCP | MCP 서버와 도구 호출 연결|
| EditorToolset | 에디터 상태·선택·뷰포트·로그 도구|
| ToolsetRegistry | 도구 등록·탐색 및 AgentSkill 관리|
| JsonUtilitiesEditor | JSON 스키마 생성에 필요한 호환 모듈|
| FileSandbox | 파일 샌드박스, ZIP 처리 및 공통 호환 코드|

### 설치 및 연결

1. UE 5.7.4의 `Engine/Plugins/UnrealMCP/` 아래에 이 저장소의 폴더 구조를 유지하여 배치합니다.
2. 에디터의 플러그인 설정에서 `EditorToolset`을 활성화하고, 요구되는 `UnrealMCP` 및 종속 플러그인도 활성화합니다. 소스 플러그인이므로 환경에 맞는 C++ 빌드 도구가 필요하며, 빌드 후 에디터를 다시 시작합니다.
3. `Model Context Protocol` 설정에서 서버 포트·경로를 확인하고 `Auto Start Server`를 활성화한 뒤 에디터를 재시작합니다. 실행 인자 `-ModelContextProtocolStartServer`로 서버를 시작할 수도 있습니다.
4. MCP 클라이언트에 **Streamable HTTP** 서버 주소 `http://127.0.0.1:8000/mcp`를 등록합니다. 포트나 경로를 변경했다면 해당 주소를 사용합니다.

```text
Engine/Plugins/UnrealMCP/
├── UnrealMCP/
├── EditorToolset/
├── ToolsetRegistry/
├── JsonUtilitiesEditor/
├── FileSandbox/
└── README.md
```

자동 서버 시작은 기본적으로 꺼져 있습니다. 클라이언트가 다른 컴퓨터에서 실행된다면 `127.0.0.1`은 해당 클라이언트 자신을 가리키므로 위 주소를 그대로 사용할 수 없습니다. 플러그인 함수 추가 후에는 재빌드·재시작하고 클라이언트의 도구 목록도 새로 조회해야 합니다.

### 제공 기능과 도구 탐색

2026-09-17 MCP 연결 확인 기준으로 다음 3개 Toolset에서 총 30개 도구를 제공합니다. 활성화된 플러그인과 버전에 따라 목록은 달라질 수 있으므로 서버의 조회 결과를 기준으로 사용하세요.

| Toolset | 도구 수| 기능|
| --- | ---: | --- |
| `EditorToolset.EditorAppToolset` | 21 | 에셋·액터 선택, 열린 에셋, 카메라, 좌표 변환, 이미지 캡처, CVar 검색, PIE 제어|
| `EditorToolset.LogsToolset` | 5 | 로그 조회·출력, 카테고리 조회, 로그 수준 조회·변경|
| `ToolsetRegistry.AgentSkillToolset` | 4 | AgentSkill 목록·내용 조회 및 생성·수정|

기본 `Enable Tool Search` 설정에서는 처음부터 모든 도구의 상세 정보를 전송하지 않습니다. 모든 MCP 클라이언트는 같은 경로로 도구를 발견하고 호출할 수 있습니다.

1. `list_toolsets` — Toolset 목록과 요약
2. `describe_toolset` — 지정한 Toolset의 도구 설명·입력 형식
3. `call_tool` — Toolset 이름, 도구 이름, 인자로 실행

선택 에셋 조회 시 `call_tool`에 전달하는 인자 예시

```json
{
  "toolset_name": "EditorToolset.EditorAppToolset",
  "tool_name": "GetSelectedAssets",
  "arguments": {}
}
```

출력 로그에 메시지를 남기는 인자 예시

```json
{
  "toolset_name": "EditorToolset.LogsToolset",
  "tool_name": "WriteLog",
  "arguments": { "message": "Hello from MCP" }
}
```

`WriteLog`는 이 백포트에서 추가한 도구입니다. 여러 줄 텍스트를 `LogMCPMessage` 카테고리의 `Display` 수준으로 기록하며, 조회 결과를 자동 기록하지 않습니다. 출력 로그에 보이지 않으면 카테고리·표시 필터를 확인하세요.

### 호환 동작과 제한

#### 콘텐츠 브라우저 선택

- 이미 열린 표준 콘텐츠 브라우저 탭 1~4를 조회합니다. 지원하는 탭 관리자에서 비활성 탭과 사이드바 탭도 조회하며, 탭을 열거나 활성화하지 않습니다.
- Project(Legacy) 소스의 선택만 읽습니다. Fab 등 다른 소스로 전환된 브라우저는 제외하며, 조회가 소스를 변경하지 않습니다.
- 중복 에셋을 제거합니다. 닫힌 탭과 별도 Content Drawer는 펼침·접힘 여부와 무관하게 제외합니다. Drawer 제외는 확인한 UE 5.8 원본의 동작과 같습니다.
- 임의 생성 브라우저나 전역·Level Editor 이외의 탭 관리자가 관리하는 브라우저는 지원하지 않아 누락될 수 있습니다. 결과는 표준 탭 번호 순으로 수집하므로 UE 5.8의 브라우저 생성 순서와 다를 수 있습니다.
- 선택 에셋 조회와 `SelectAssets` 완료 확인에 같은 방식이 적용됩니다. 이 탐색은 UE 5.7.4의 위젯 구조를 기준으로 합니다.

#### 뷰포트 및 상태 조회

월드·화면 좌표 변환은 UE 5.8 원본처럼 첫 Perspective 레벨 뷰포트를 기준으로 합니다. 5.7.4용 투영·역투영 API를 사용하며, 유효하지 않은 뷰포트에서는 실패와 초기화된 출력값을 반환합니다.

`EditorToolset.EditorAppToolset`에 인자 없는 `GetEngineVersion`과 `GetCurrentLevel`을 추가했습니다. 각각 실행 중인 엔진의 전체 버전 문자열과 에디터 월드의 맵 패키지 경로를 직접 반환합니다. 레벨은 Persistent 맵 기준이며, 활성 서브레벨이나 PIE 월드를 의미하지 않습니다. 저장하지 않은 맵은 임시 경로일 수 있습니다. 재빌드·재시작 후 사용 가능하며, 두 신규 도구는 아직 빌드·실행 검증 전입니다. 위 실측 30개 도구에 두 개가 추가되는 구성입니다.

시작 구간 로그 조회가 비어 있으면 `GetLogEntries`의 `category`를 빈 문자열로 명시하고 `pattern`으로 검색하세요. 타임스탬프 없는 시작 로그는 카테고리 필터에서 누락될 수 있습니다.

### 확인된 상태

2026-09-17 기준

- UE 5.7.4 빌드 성공은 개발 환경에서 확인되었습니다.
- MCP 연결·도구 탐색·선택 에셋 조회·로그 조회·`WriteLog` 기록 및 재조회가 실제 세션에서 확인되었습니다.
- 모든 도구, 숨김 탭·Fab 등의 전체 조합, 패키징, 이전 엔진 버전까지 검증한 것은 아닙니다.
