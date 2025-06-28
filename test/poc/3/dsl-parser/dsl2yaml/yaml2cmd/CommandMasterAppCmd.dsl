Commands
Group UnnamedGroup
Command list_files alias:ls
Description: List files in a directory
Args: directory
Out: filelist
Shell: ls "{directory}"

Command find_pattern alias:grep
Description: Find a pattern in a file list
Args: pattern in:filelist
Out: matches
Shell: echo "{in}" | grep "{pattern}"

Command count_matches alias:count
Description: Count number of matches
Args: in:matches
Out: count
Shell: echo "{in}" | wc -l

