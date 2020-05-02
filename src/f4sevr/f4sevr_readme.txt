Fallout 4 Script Extender VR v0.6.20 beta
by Ian Patterson, Stephen Abel, and Brendan Borthwick (ianpatt, behippo, and plb)

The Fallout 4 Script Extender VR, or F4SEVR for short, is a modder's resource that expands the scripting capabilities of Fallout 4 VR. It does so without modifying the executable files on disk, so there are no permanent side effects.

Fallout 4 VR is unplayable with the null VR driver, and I do not have VR hardware, so it is impossible for me to extensively test or support this release. This may be EXTREMELY broken, but probably isn't.

Compatibility:

F4SEVR will support the latest version of Fallout available on Steam, and _only_ this version. When a new version is released, we'll update as soon as possible: please be patient. Don't email asking when the update will be ready; we already know the new version is out. The editor does not currently need modification, however when available a custom set of .pex/psc files must be installed.

This version is compatible with runtime 1.2.72. You can ignore any part of the version number after those components; they're not relevant for compatibility purposes.

[ Installation ]

1. Copy f4sevr_1_2_72.dll, f4sevr_loader.exe, and f4sevr_steam_loader.dll to your Fallout installation folder. This is usually C:\Program Files (x86)\Steam\SteamApps\common\Fallout 4 VR\, but if you have installed to a custom Steam library then you will need to find this folder yourself. Copy the Data folder over as well.

2. Launch the game by running the copy of f4sevr_loader.exe that you placed in the Fallout installation folder.

3. If you are looking for information about a specific feature, check f4sevr_whatsnew.txt for more details.

[ FAQ ]

* Console version?
 - No. Not possible due to policy restrictions put in place by the console manufacturers.

* My virus scanner complains about f4sevr_loader. Is it a virus?
 - No. DLL injection sets off badly-written virus scanners. Nothing we can do about it.

* Can I redistribute or make modified versions of F4SEVR?
 - No. The suggested method for extending F4SEVR is to write a plugin. If this does not meet your needs, please email the contact addresses listed below.

* How do I write a plugin for F4SEVR?
 - There is no example plugin yet, so you'll need to look at PluginAPI.h for details on the high-level interface. The API is not currently locked down due to the early state of the project, so anything may change in later versions. Note that plugins must have their source code publicly available.

* How do I uninstall F4SEVR?
 - Delete the three files you copied to your Fallout installation folder.

* How much does this cost?
 - F4SEVR is free.

[ Contact ]

DO NOT EMAIL FOR TECH SUPPORT. THIS IS AN UNSUPPORTED RELEASE. PLEASE DON'T BOTHER OTHER TEAM MEMBERS ABOUT THIS.

Ian (ianpatt)
Send email to ianpatt+f4sevr [at] gmail [dot] com

[ Standard Disclaimer ]

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
