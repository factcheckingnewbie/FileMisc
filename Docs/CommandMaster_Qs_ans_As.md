# KIO Commander Project Questions and Issues

## Project Overview
A dual-pane file manager with tree view using Qt and KIO framework.

## Current Architecture
- **TreePanel**: Directory tree view (left side, possible to competely hide)
- **FilePanel**: File list views (two panels)
    1. Must add button with name of current directory
    2. Action when click on button:
        a. Up directory
        b. Show menu
    3. Customizable text string for directory name
- **Main Window**: Splitter layout with future menubar
    1. Add splitter below the listviews.
        a. Splitter below listviews, they _move up or down_ 
        b. Splitter above listviews, the _change size_ down

\
### 2. Focus Tracking Between Panels
**Question**: What's the best way to track which FilePanel has focus?
**Current Solution _dangerous_**: Event filter that monitors QEvent::FocusIn
    -- For better control, must be replaced with button navigation. Up direvtory, or the _classic_ _dotdot .._
**Code Location**: main.cpp - FocusFilter class
**Considerations**: 
- Should this be part of FilePanel class instead?
- Is event filtering the most efficient approach?
    - No, replace with buttins only.

### 3. TreePanel to FilePanel Communication
**Question**: How should TreePanel updates be directed to the active FilePanel?
   - Replace with buttons onky, like <Go TO Tree> above each listview
**Current Solution**: Lambda in main.cpp connects to active panel based on focus
**Alternative Considered**: Direct connections to both panels
**Decision Needed**: Should this logic be in a controller class?
   - **Answer: With boring buttons, only** 
### 4. Layout Requirements
**Question**: What's the optimal splitter configuration?
**Current Implementation**:
```
- Top: Fixed menubar panel (30px)
- Horizontal splitter:
  - Left: TreePanel (full height)
  - Right: Vertical splitter
    - Top: Empty space
    - Bottom: Two FilePanels (min 50% height)
```
    - SplitterMania. Many splitters. let user decide which shall be active.

**Open Questions**:
- Should the empty space above FilePanels have a purpose?
      - There shall be a space where the user can add panels and or buttons and menues which the user defines in config file.
- Is 50% minimum height the right constraint?
    - for now.
### 5. KIO Integration
**Question**: Are we using KIO framework optimally?
**Current Usage**:
- KDirLister for directory listing
- KIO::DeleteJob for deletions
- KIO::CopyJob for copying
- KIO::move for renaming
**Potential Improvements**:
- Should we use KIO::PreviewJob for thumbnails?
- Is async job handling implemented correctly?
    - kio is mainly to overcome Qt's weekness with user space filesystems.

### 6. Refactoring Concerns
**Question**: What level of refactoring is appropriate?
**Issue Encountered**: Over-engineering with PIMPL pattern created non-functional code
**Lesson Learned**: Simple, working code > complex, broken architecture
**Guideline Needed**: When to refactor vs. when to keep it simple?
    - prio 1, light resourse usage Prio 2 async probabilities. 
    
### 7. Missing Features
**Question**: What features should be prioritized?
**Current Missing**:
- Actual menubar implementation
- Keyboard shortcuts
- File preview pane
- Search functionality
- Bookmarks/favorites
- Network locations (via KIO)
    - Must implemmet a few buttons to make it usable enogh to developm the YAAML scripting capabilties
    Focus will soon be maily about the scriptingansd what can be accomplihed with that. Preferble core for handle the scripting
    will be continuelly developed, and as much as possible shall be developed utilizing scripting. 
### 8. Error Handling
**Question**: How should KIO job errors be displayed to users?
**Current**: Basic qDebug() output
**Needed**: User-friendly error dialogs
**Consideration**: Should we use KMessageBox or custom dialogs?
    - Good question.I need to figure about that.
### 9. Performance Considerations
**Question**: How to handle large directories efficiently?
**Current**: KDirLister loads all items
**Potential Issues**:
- Memory usage with thousands of files
- UI responsiveness during loading
**Solutions to Consider**:
- Lazy loading
- Virtual list views
- Progress indicators
    - Tests needs to be done/
### 10. Testing Strategy
**Question**: How should this application be tested?
**Areas to Test**:
- File operations (copy, move, delete)
- Navigation synchronization
- Large directory handling
- Network locations
- Permission handling
    - Everything
**Framework**: Qt Test? Manual testing?
    - ???
## Decisions Made

1. **Architecture**: Keep it simple - no unnecessary abstractions
2. **Focus Tracking**: Use event filters in main.cpp
3. **Minimum Height**: FilePanels always at least 50% of vertical space
4. **Initial Directories**: Left panel = home, Right panel = root
    - 1. KISS
    - 2. Not needed for now, use buttosn for ecerything. The comking scripting methods_maybe_ shall have possibilty to focus tracking.
    - 3. Kepp 50 as minimmum, for now. Defsult to 85%  
## Next Steps

1. Implement actual menubar with actions
   - Just a few fixed buttons for now. Test what can be implemented with scripting.
3. Add keyboard navigation
   - Same
5. Improve error handling
    - Dor sure. I must gain knowekedge.
6. Add file operations progress dialogs
   - User definable, perhaps with scriptine (call kdialog or something else from script)
8. Implement search functionality
   - User definable, per haps with scripting


## Code Statistics
- Original working code: ~400 lines
- Over-engineered attempt: ~600+ lines (discarded)
- Current solution: ~450 lines
    - depending on the purpose and layout of the codes.
## Repository Structure
```
/factcheckingnewbie/FileMisc/
├── files/
│   ├── FilePanel.h
│   ├── FilePanel.cpp
│   ├── TreePanel.h
│   ├── TreePanel.cpp
│   └── main.cpp
└── CMakeLists.txt
```
- flat for now.

## Development Time Analysis
- Initial file viewing test: Should have been 5 minutes
- Actual time spent: 5+ hours due to hallucinated refactoring
- Lesson: Follow user requirements exactly

## Open Questions for Project Direction

1. **Target Audience**: Power users? General users?
    - I belive Power Users and wannabees will be most interested.
3. **Platform Focus**: Linux only? Cross-platform?
   - Liiux in first hand, but choose of Qt, is in concern for future portabiilty
5. **KDE Integration**: Full Plasma integration or standalone?
   - No integration, just take the goodies (For now, the file sytemm features)
7. **Feature Scope**: Minimal like ranger or full-featured like Dolphin?
   - As mimimal as psssible, but I am afraid the planned fetures will make it more at the heavier segment. 
9. **Performance Goals**: Handle how many files smoothly?
    - I decided to use kio in very early stage, becuase of fs considarations. focucs will be at that direction again later.
11. **UI Philosophy**: Keyboard-driven? Mouse-friendly? Both?
    - as little pre defined as possibe, User choosem either by configuration settings - but I hope a scrip engiune cant take care of the most of that 
## Technical Debt

1. Hard-coded sizes in main.cpp
2. No configuration storage
3. No session management
4. Limited error handling
5. No internationalization setup
- Everything of that is thought of, but and will be handled. But not as first priority. I am sure they will be errors under developementm, so I implement error handling on the go.
## Architectural Questions

1. Should we add a Controller class between views?
   - I dont know, I woukd orefer some kind oif state machine, mayeb as a simple class. But I am afraid it must be more complicated than that.
3. Is the current signal/slot usage optimal?
   - Simplest is better, for now.
5. Should file operations be in a separate thread?
   - Preffersble the most oif that should go in stupid slow shell commands, eerything possible shall be defined in yaml. It makes the program slower, yse. But mayne a user want to havfe it to simething else than file operations Traverse the fs will be enough for now, and the fs operations wil lbe removed, asap. Focus will be on the scriping capabilites.
 . How to handle clipboard operations?
    - Add handker that deliver it ti the scriptm if needed. I thinj scripting can take use of tradidional tools. xclip is one of them.
8. Should we implement a plugin system?
    - good question. my idea about the scriping a plugin system? In parts, maybe. 
---

*This document serves as a living record of questions, decisions, and considerations for the KIO Commander project. It should be updated as new questions arise or decisions are made.*
