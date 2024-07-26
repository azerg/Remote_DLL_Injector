Remote Stealth DLL Injector
========

C++ DLL Injector ( old codes from 2012 :D ).

### todo:
* injected dll might use c++ exceptions.
* fill caves between injected dll's sections with random bytes
* x64 support lol

## Usage
Sample code ships with dummy ***InjectorUI***.

UI:
* ***Process name*** - executable name ( e.g. targetApp.exe )

Options:
* ***remove PE-header*** - cleans PE-header after injection to preserve memory-scan detection
* ***random head\random tail*** - adds extra random bytes to the beginning\end of injected module
* ***remove extra sections*** - clears ".rsrc" & ".relocs" sections after injection
* ***inject with local dll*** - Appending our dll-to-inject codes to a dummy (100% clean) dll. After injection, stub will execute our dll. Dummy dll is used to bypass analysis of injected executable if exists.
