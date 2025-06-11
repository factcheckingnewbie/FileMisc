# Project Idea: CommandMaster

## 1. Purpose & Vision

CommandMaster aims to be a next-generation, customizable file and system management tool.  
Its core idea is to replace tedious command-line management and ad-hoc scripting with a flexible, user-driven GUI.  
Users can define, group, and safely execute custom shell commands (and more) through simple YAML configuration, all from a powerful and context-aware interface.

---

## 2. Core Features

- **Basic File Management:** Two-panel file manager functionality (copy, move, rename, etc.)
- **Replacing Tedious CLI Work:** Users can perform complex or repetitive shell tasks without manually typing commands or writing scripts.
- **No Scripting Required:** Common tasks can be described in YAML, removing the need for bash/python scripting for routine operations.
- **Custom Command Groups:** Commands are grouped (and optionally sub-grouped) by category (e.g., file management, SELinux, btrfs, containers, etc.).
- **Menu-driven or Alias-driven:** Users can select commands from a menu or invoke by typing an alias.
- **YAML-Driven:** All commands, groupings, argument prompts, context (sudo, docker, etc.), and permissions are declared in YAML files.
- **Context Awareness:** Command context (plain, sudo, docker, etc.) can be set per-command or per-invocation.
- **User Arguments:** By default, arguments are entered manually via a command line. Optionally, the YAML can specify that the UI should prompt for required arguments.
- **Protected Commands:** Certain commands/aliases can be marked as protected (not overridable, requiring confirmation, or restricted to certain users).
- **Alias Management:** Aliases can be used as menu entries, CLI shortcuts, or both. Namespacing and disambiguation are supported.
- **Auditing:** All command executions, especially privileged ones, are logged with group and alias details.
- **Extensible:** New command groups and workflows can be added by simply editing YAML files.

---

## 3. Alias Ambiguity & Security

### The Problem

If two (or more) command groups define the same alias (e.g., `reboot`), ambiguity arises.  
This can lead to user error or, worse, accidental execution of privileged or destructive commands.

### Raw Solution & UI Design

#### 1. **Alias Uniqueness Enforcement**

- By default, aliases should be unique across all groups.
- Configurable policies:
  - "Unique aliases only" (strict)
  - "Allow ambiguous aliases with popup prompt" (relaxed)
  - "Namespacing required" (secure/power-user mode)

#### 2. **Namespace Support**

- Allow commands to be invoked as `group:alias` (e.g., `system:reboot` or `docker:reboot`).
- In the UI, show the full path/group for all commands.

#### 3. **Ambiguity Popup**

- If the user invokes an ambiguous alias, CommandMaster displays a popup listing all matching groups.  
  The user must select the intended command.  
  Popup should highlight any protected/dangerous commands.

#### 4. **Current Group Feedback**

- The current command group (or context) should always be displayed in the UI (status bar, header, etc.)  
  Example: `Current group: docker`
- This helps prevent mis-execution due to context confusion.

#### 5. **Protected Aliases**

- Protected commands cannot be overridden or invoked without explicit group prefix and confirmation.
- For highly privileged or dangerous commands, require both group:alias syntax and extra confirmation.

#### 6. **YAML Validation**

- On loading YAML, CommandMaster checks for ambiguous aliases and enforces policy (error, warn, allow).
- For protected aliases, enforce additional checks and permissions.

#### 7. **Audit Logging**

- All executions are logged with timestamp, user, group, alias, arguments, and context.

---

## 4. YAML Example

```yaml
- group: system
  label: System Operations
  commands:
    - alias: reboot
      command: reboot
      protected: true

- group: docker
  label: Docker Management
  commands:
    - alias: reboot
       # Prompt for password, or other security mechanisms
      command: docker restart $(container)
      args:
        - container
      protected: false
```

---

## 5. UI/UX Behavior

- **Command Line Example:**  
  User types: `reboot`
- **Ambiguity Detected:**  
  Popup:
  ```
  Multiple commands match 'reboot':  
  [ ] system:reboot - System Operations (protected!)
  [ ] docker:reboot - Docker Management
  Please select which command to run.
  ```
- **Direct Namespaced Invocation:**  
  User types: `docker:reboot mycontainer`
- **Current Group Displayed:**  
  UI shows: `Current group: docker`
- **No Prompts Unless Configured:**  
  If `prompt_args` is false in YAML, no argument dialogs appear; the command line is always available.

---

## 6. Additional Design Considerations

- **Popup fatigue:** Minimize unnecessary popups by encouraging unique aliases or explicit namespacing.
- **Security:**  
  - Prevent command injection via argument validation and shell-escaping.
  - Restrict editing or overriding of protected commands/aliases.
  - Prompt for authentication when running sudo/contextual commands.
- **Extensibility:**  
  - Users/admins can add new YAML files for new domains (SELinux, btrfs, containers, etc.)
  - CommandMaster can be adapted for any workflow that benefits from automatable, repeatable command invocation.
- **User power:**  
  - Users can paste full commands, use aliases, add args, and select context—all in one CLI-like bar.
  - GUI argument prompting is optional, not mandatory.

---

## 7. Critique

**Strengths**
- Highly flexible and user-driven.
- Fills a real gap between pure CLI and rigid GUI tools.
- YAML-based configuration is approachable for non-programmers.
- Namespacing and audit logging provide strong safety nets.

**Weaknesses**
- Alias ambiguity is inherently risky if not managed; must not rely only on popups.
- Security is complex—must be airtight to avoid privilege escalation or command injection.
- Could become overwhelming with too many groups/aliases unless UI is carefully designed.
- Popups are a last resort: design should favor structural clarity over runtime disambiguation.
- YAML config can become complex if not carefully structured.

---

## 8. Summary

- **CommandMaster** empowers users to manage systems/files/containers/anything by composing, grouping, and safely executing shell commands from a rich GUI.
- **Ambiguity and security** are handled via namespacing, UI feedback, policy enforcement, and audit logging.
- **No reasoning, code, or critique omitted.**
