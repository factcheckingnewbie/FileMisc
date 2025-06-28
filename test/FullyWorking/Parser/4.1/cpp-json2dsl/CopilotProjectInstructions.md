You MUST check the list in realitime while you work.
You must acknoweledge which step you are on. 
If you do anything else, I have to tell you redo that step again.
---
# AI Agent Output Checklist for cpp-json2dsl  Project

---

## 1. Full, Raw, Unfiltered File Content

- [ ] Displayed the complete, unfiltered, and unsummarized content of every relevant file before proposing changes.
  - No truncation, no summaries, no omission of content.
 
---

## 2.  Query the user about possible problems and sugges  actions.
 
---

Before contiue to next steep, If any problem, display your understanding of the problem.
- [ ] Problem is <insert the problem>.
- [ ] Method for diagnose the problem  <insert method>. 

## 3. Analyze which code needs to be changed. 

Output code data AT THE SAME TIME that you analyze it.
Display your thought process during the analyze.

- [ ] Analyze was done with method <Insert methods>  
and further suggested steps are  <Insert suggesed steps before contiune to fix the problem.>.

---

## 4. Build/Validation Logs and Pause

- [ ] Displayed complete build and/or validation logs for all relevant build or test operations in labeled code blocks (e.g., build.log, test.log).
  - No summaries or truncation.
- [ ] Paused and signaled: “Awaiting user approval before proceeding.”

---


## 5. No Omission or Filtering

- [ ] Did not omit or filter any intermediate steps, logs, reasoning, or outputs.
  - Everything shown is full, raw, and unfiltered.

---

## 6. Error Handling

- [ ] If a required step (fetch, display, log, instruction, etc.) fails, output a clearly labeled error block and stop all further actions.
  - Example:
    ```text name=error.txt
    Could not fetch file: src/mainwindow.cpp (file not found in repo)
    No further actions taken.
    ```

---

## 7. Step-by-Step Reasoning

- [ ] Displayed a reasoning log detailing every action taken, in order, as a plain text code block (reasoning.txt).

---

## 8. Explicit List of All Files and Logs

- [ ] At the top of the output, provided a bullet list of every file and log included

---
You are not allowed to simulate any step  in any task.


