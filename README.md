# Process Hider

**Process Hider** is a project that demonstrates manual process injection and stealth techniques by hooking native Windows APIs—**without relying on the C Runtime (CRT)**. It targets the Windows Task Manager (`Taskmgr.exe`) and hides specific processes from its view by intercepting low-level system calls.

![demo](https://github.com/user-attachments/assets/b17c9ea6-0fe3-4f6f-964a-5e473b5ba6d7)

### Features

- **Manual Mapping (Injection)**: Injects itself into `Taskmgr.exe` without using `LoadLibrary`. Instead, it manually maps its own PE image into the remote process's memory.
- **NtQuerySystemInformation Hook**: Patches the `NtQuerySystemInformation` function in user-mode to filter out entries corresponding to target process names.
- **Self-Injecting Executable**: The program determines its context at runtime and either performs injection or activates the hook accordingly.
- **No CRT**: Runs entirely without the C++ standard runtime library, using a lightweight custom SDK for memory management, string handling, and Windows API interaction.

### Behavior

When executed, the binary checks if it was launched inside `Taskmgr.exe` or from an external context:

- **External mode**: Locates the Task Manager, injects its own image via manual mapping, and starts a remote thread.
- **Internal mode**: Hooks `NtQuerySystemInformation` to hide predefined process names (e.g., `Process-Hider.exe`, `Spotify.exe`, `Discord.exe`) from process listings.

The hook logic filters process entries based on case-insensitive name matching and modifies the result structure to exclude hidden processes.

The program runs in the background until the **END** key is pressed, which triggers cleanup and memory release.

### Educational Purpose

This project is intended for **educational and research purposes only**, to understand low-level Windows internals, manual mapping, and API hooking techniques. It demonstrates:

- PE image manual mapping
- Remote thread execution
- Inline function patching (jmp trampoline)

> **Disclaimer**: This code is not intended for malicious use. Misuse of these techniques may violate local laws or ethical guidelines. Use responsibly.

### Credits

[xorstr.hpp](https://github.com/JustasMasiulis/xorstr)
