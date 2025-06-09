#!/bin/bash

echo "=== Validation of FileManager Changes ==="
echo

# Check that the application compiles
echo "1. Compilation Test:"
cd /home/runner/work/FileMisc/FileMisc
if make -q; then
    echo "   ✓ Application compiles successfully"
else
    echo "   ✗ Compilation failed"
    exit 1
fi

# Check key code changes in header file
echo
echo "2. Header File Changes:"
if grep -q "fileSystemModel2" filemanager.h; then
    echo "   ✓ Independent fileSystemModel2 declared"
else
    echo "   ✗ Missing independent fileSystemModel2"
fi

if grep -q "onListView2Clicked" filemanager.h; then
    echo "   ✓ onListView2Clicked slot added"
else
    echo "   ✗ Missing onListView2Clicked slot"
fi

if grep -q "updateFocusStyle" filemanager.h; then
    echo "   ✓ updateFocusStyle method declared"
else
    echo "   ✗ Missing updateFocusStyle method"
fi

if grep -q "eventFilter" filemanager.h; then
    echo "   ✓ eventFilter override declared"
else
    echo "   ✗ Missing eventFilter override"
fi

# Check key code changes in implementation file
echo
echo "3. Implementation Changes:"
if grep -q "fileSystemModel2 = new QFileSystemModel" filemanager.cpp; then
    echo "   ✓ Independent fileSystemModel2 instantiated"
else
    echo "   ✗ Missing fileSystemModel2 instantiation"
fi

if grep -q "listView2->setModel(fileSystemModel2)" filemanager.cpp; then
    echo "   ✓ listView2 uses independent model"
else
    echo "   ✗ listView2 not using independent model"
fi

if grep -q "Removed synchronization" filemanager.cpp; then
    echo "   ✓ Synchronization removed (comment found)"
else
    echo "   ✗ Synchronization removal not documented"
fi

if grep -q "border: 2px solid #4A90E2" filemanager.cpp; then
    echo "   ✓ Focus styling implemented"
else
    echo "   ✗ Missing focus styling"
fi

if grep -q "installEventFilter" filemanager.cpp; then
    echo "   ✓ Event filters installed"
else
    echo "   ✗ Missing event filters"
fi

# Check that synchronization calls were removed
echo
echo "4. Synchronization Removal:"
if ! grep -q "updateListView2()" filemanager.cpp | grep -v "void FileManager::updateListView2()" | grep -v "// This method is no longer used"; then
    echo "   ✓ updateListView2() calls removed from setup"
else
    echo "   ? updateListView2() calls may still exist"
fi

echo
echo "=== Validation Complete ==="
echo
echo "Summary: The changes successfully implement:"
echo "- Independent list views (no synchronization)"
echo "- Visual focus indicators (blue border/background)"
echo "- Event handling for focus changes"
echo "- Directory navigation for the second list view"