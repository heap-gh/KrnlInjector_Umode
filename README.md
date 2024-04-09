# Manual Mapper Usermode Interface

## Introduction

This Manual Mapper Usermode Interface provides a user-friendly graphical interface built with ImGui for interacting with the Manual Mapper tool. It facilitates the selection of processes by name or ID, communication with the `kernel_injector` driver, and the option to hijack a handle.

## Features

- Select processes either by name or ID.
- Seamless communication with the `kernel_injector` driver.
- Handle hijacking functionality for advanced users.
- Simple and intuitive interface built with ImGui.

## Requirements

- `kernel_injector` driver must be installed and operational.
- Compatible operating system (Windows recommended).
- ImGui library integrated for graphical interface.
- Access to process information.

## Usage

1. Launch the Manual Mapper Usermode Interface.
2. Choose a process to inject by either entering its name or ID.
3. Select the desired injection method.
4. Optionally, enable handle hijacking if required.
5. Initiate the injection process.
6. Monitor the progress and any status messages.
7. Once injection is complete, verify the injected module in the target process.

## Dependencies

- ImGui: [https://github.com/ocornut/imgui](https://github.com/ocornut/imgui)
- `kernel_injector` driver: [Link to driver repository or installation instructions]

## Installation

1. Clone or download the repository containing the Manual Mapper Usermode Interface.
2. Ensure that ImGui library is included and set up properly.
3. Ensure that the `kernel_injector` driver is installed and configured on the system.
4. Compile and build the Manual Mapper Usermode Interface.
5. Execute the compiled executable.

## Configuration

No specific configuration is required for the Manual Mapper Usermode Interface. However, ensure that the `kernel_injector` driver is correctly installed and accessible.

## Troubleshooting

- If encountering issues with injection, ensure that the `kernel_injector` driver is loaded and functioning correctly.
- Check for any error messages or logs generated during the injection process.
- Ensure that the selected process is valid and accessible.

## License

[Include license information for the Manual Mapper Usermode Interface]

## Acknowledgments

- ImGui for providing an excellent library for creating graphical interfaces.
- Developers and contributors to the `kernel_injector` driver.

## Contact

For any inquiries or support, please contact [provide contact information].
