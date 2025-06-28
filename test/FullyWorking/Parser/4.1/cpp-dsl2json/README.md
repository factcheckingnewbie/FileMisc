# cpp-dsl2json Project

## Overview
The `cpp-dsl2json` project is a command-line application that parses a Domain-Specific Language (DSL) and generates JSON output. The parser is designed to maintain the original structure of the DSL, including index order, whitespace for indentation, and comments, ensuring 100% roundtrip compatibility.

## Features
- Command-line parser for DSL input.
- Generates JSON output while preserving formatting and comments.
- Easy to extend and modify for additional DSL features.

## Project Structure
```
cpp-dsl2json
├── src
│   ├── main.cpp          # Entry point of the application
│   ├── dsl_parser.cpp    # Implementation of the DSLParser class
│   ├── dsl_parser.h      # Header file for the DSLParser class
│   ├── utils.cpp         # Utility functions for parsing
│   └── utils.h          # Header file for utility functions
├── include
│   └── json_writer.h     # Header file for the JsonWriter class
├── CMakeLists.txt        # CMake configuration file
└── README.md             # Project documentation
```

## Building the Project
To build the project, you need to have CMake installed. Follow these steps:

1. Clone the repository or download the project files.
2. Open a terminal and navigate to the project directory.
3. Create a build directory:
   ```
   mkdir build
   cd build
   ```
4. Run CMake to configure the project:
   ```
   cmake ..
   ```
5. Build the project:
   ```
   make
   ```

## Running the Application
After building the project, you can run the application from the terminal:

```
./cpp-dsl2json <path_to_dsl_file>
```

Replace `<path_to_dsl_file>` with the path to your DSL input file.

## Example Usage
Given a DSL file `example.dsl`:

```
Window "Shell Commander" width:1024 height:768
Panel menubar position:top color:green
```

You can run the application as follows:

```
./cpp-dsl2json example.dsl
```

The output will be a JSON representation of the DSL input, preserving the original formatting and comments.

## Contributing
Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.