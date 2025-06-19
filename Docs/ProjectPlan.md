# CommandMaster Project Plan


 *   Directory button implementation proposal 
	 - Prio 1
 *   Visual focus indication proposal
	 - Prio 2
 *    Additional splitter layout proposal
    	- Prio Later
 *    Go To Tree buttons proposal
 	 -  Prio 3 
 *   Configuration system YAML structure
 	- Prio 4 And Final goal for today.
 *   Script execution system design
 	 - Prio 5 
 *    Operation toggle system (for future)
 		- ???
 *   Minimal features list

##  Details 
-    Highest priority. Without these change, commands to files will be unpredictable
	___
		-    Add punktpunkt to listviews
		-  Add panel at top  of each listview .
	 Panel shall have one buttine with  text field for each listciew that show truncated string of  current directory
 
 		-  Action when click on button:
			- Show menu defined in yaml
			-  Development on menu will continue in Yaml tigether woth creating  a new  ScriptingEngine. 
			- First menu-item shall be rename-file, which will issue 
		```
		// pseudocode
		Fname=current selected_file_in_listview1
		mv file_in_listview2 Fname
		```
	That gives, Fiirst step to create scripting engine is to 
	implement an method to query CommandMaster for what is selected in the panels.	

 -   The above will in future need changes to placements and layouts. For now, as a poc - it will due.	
### Must Be Done, but not First priority . 
   -  a  (Is implented) Splitter above listviews, the _change size_  down or up
     - b.  (ToBeImplented)  Splitter below listviews, they _move_ up or down 

### _Must_Fix_List_.  Will be  parallel to the rest off  project plan.
1. Must Fix  Hard-coded sizes in main.cpp
2. No configuration storage
3. No session management
4. Limited error handling
5. No internationalization setup


## Development Time Analysis
- Initial file viewing test: Should have been 5 minutes
- Actual time spent: 5+ hours due to hallucinated refactoring
- Lesson: Follow user requirements exactly


##
*This document serves as a plan list and will continues  be extracted from kio-commander-questions.*
